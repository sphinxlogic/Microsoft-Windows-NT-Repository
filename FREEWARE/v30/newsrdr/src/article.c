/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Article-related routines.
**
**  MODULE DESCRIPTION:
**
**  	This module contains routines that deal directly with article
**  handling.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, 1994 MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  06-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	06-SEP-1992 V1.0    Madison 	Initial coding.
**  	13-SEP-1992 V1.0-1  Madison 	Slight cache management improvements.
**  	13-OCT-1992 V1.0-2  Madison 	Work around $FAO !AZ problem.
**  	16-OCT-1992 V1.0-3  Madison 	Fix signature file line count.
**  	15-FEB-1993 V1.0-4  Madison 	Fix LIB$SIGNAL references.
**  	15-FEB-1993 V1.1    Madison 	Replace Mark_Xref_Seen with Mark_Xref.
**  	12-APR-1993 V1.2    Madison 	Add Edit_Article.
**  	22-AUG-1993 V1.2-1  Madison 	Trim trailing blanks from header lines.
**  	21-SEP-1993 V1.3    Madison 	Add authentication support.
**  	22-SEP-1993 V1.4    Madison 	Add XHDR support.
**  	23-SEP-1993 V1.5    Madison 	Add Check_For_Holes, etc.
**  	28-SEP-1993 V1.5-1  Madison 	Fix Get_One_Header header allocation.
**  	30-SEP-1993 V1.5-2  Madison 	Another fix for XHDR support.
**  	02-OCT-1993 V1.5-3  Madison 	Fix another bug in Get_Article_Hdrs.
**  	09-OCT-1993 V1.6    Madison 	Fix up header.
**  	26-OCT-1993 V1.6-1  Madison 	Another XHDR fix.
**  	15-JAN-1994 V1.6-2  Madison 	Handle nosucharticle status differently.
**  	16-MAR-1994 V1.7    Madison 	Add support for kill by message-id.
**  	13-APR-1994 V1.8    Madison 	Start of XOVER support.
**  	29-APR-1994 V1.8-1  Madison 	Fixup of XHDR & XOVER support.
**  	25-MAY-1994 V1.8-2  Madison 	Wasn't caching (none) replies on XHDR.
**  	01-JUN-1994 V1.8-3  Madison 	Fix ACCVIO processing bad XOVER response.
**  	06-JUL-1994 V1.8-4  Madison 	Fix another XOVER bad-reply ACCVIO.
**  	09-JAN-1995 V1.8-5  Madison 	Fix XHDR bad-reply ACCVIO.
**--
*/
#include "newsrdr.h"
#include "globals.h"

    struct ART {
    	struct QUE *hdrqptr;
    	unsigned int bodyunit;
    	int lines;
    	int hlines;
    	int remlines;
    	int status;

#define ART_K_STATUS_RETRIEVED	0
#define ART_K_STATUS_IN_HEADERS	1
#define ART_K_STATUS_IN_BODY	2
#define ART_K_STATUS_DONE   	3

    	struct HDR *hlnptr;
    	int artnum;
    	struct GRP *grpptr;
    	char bodyfspec[FSPEC_SIZE];
    };

#define XHDR_COUNT 7
#define XHDR_MASK (~((-1)<<XHDR_COUNT))
    static int xhdr_codes[XHDR_COUNT] = {
    	NEWS_K_HDR_SUBJECT, NEWS_K_HDR_FROM, NEWS_K_HDR_KEYWORDS,
    	NEWS_K_HDR_LINES, NEWS_K_HDR_DATE, NEWS_K_HDR_MESSAGE_ID,
    	NEWS_K_HDR_REFERENCES};
#define XHDR_CODE_LINES 3

/*
**  The values in this vector are indices into the xhdr_codes vector.
*/
    static int xover_hdrs[] = {0, 1, 4, 5, 6, -1, 3};

    struct CHDR {
    	struct CHDR *flink, *blink;
    	struct QUE hdrq;
    	struct HDR *xhdrs[XHDR_COUNT];
    	unsigned int have_xhdrs;
#define CHDR_M_SUBJECT	(1<<0)
#define CHDR_M_FROM 	(1<<1)
#define CHDR_M_KEYWORDS	(1<<2)
#define CHDR_M_LINES	(1<<3)
#define CHDR_M_DATE 	(1<<4)
#define CHDR_M_MESSAGE_ID (1<<5)
#define CHDR_M_REFERENCES (1<<6)
    	int have_all_headers;
    	int artnum;
    	int hlines;
    };
/*
** Forward declarations
*/
    unsigned int Retrieve_Article(int, struct ART **, char *, char *, char *);
    unsigned int Show_Article_Page(struct ART **);
    void         Rewind_Article(struct ART **);
    void         Close_Article(struct ART **);
    static void  wipe_article(struct ART **);
    int          Mark_Article_Seen(struct GRP *, int);
    int          Mark_Article_Unseen(struct GRP *, int);
    void         Mark_Xref(char *, int);
    int          Next_Unseen_Article(int, int, struct GRP *);
    int          Count_Unseen(struct GRP *);
    unsigned int Post_Article(struct QUE *, char *, int);
    unsigned int Get_Article_Hdrs(struct GRP *, int, struct QUE **, int, int *);
    unsigned int Get_One_Header(struct GRP *, int, int, struct HDR **);
    int	    	 Preload_Cache(struct GRP *, int, int);
    unsigned int Get_Article_Body(int, char *, char *, int *, int);
    int          Ignore_Article(struct GRP *, int);
    unsigned int Edit_Article(struct ART **);
    unsigned int Article_ExH(void);
    void    	 Check_For_Holes(struct GRP *, int, int);
    int	    	 Is_Valid_Article(struct GRP *, int);
    int	    	 Article_Seen(struct GRP *, int);
    static void	 Cache_Init(struct GRP *);
    static struct CHDR *Cache_Lookup(int);
    static struct CHDR *Cache_Add(struct CHDR *, int, int);

    EXTERN struct GRP *curgroup;
    EXTERN int        rotate_text;
    EXTERN int	      Read_Full_Headers;
    EXTERN int	      pb_cols;

    static struct ART *artsave;
    static struct QUE hdrcache = {&hdrcache, &hdrcache};
    static struct GRP *hcgroup = NULL;
    static int hccount = 0;
    static int hdrcache_remove_from_tail = 0;

    extern void Parse_Xref(char *, struct QUE *);
    extern struct GRP *Find_Group(char *);
    extern void Make_Return_Address(char *, int);
    extern unsigned int Parse_Headers(struct QUE *, struct QUE *, int *);
    extern void mem_initcache(int);
    extern struct CHDR *mem_getcache(void);
    extern void mem_freecache(struct CHDR *);
    extern void mem_delcache(void);
    extern unsigned int get_cmd_noecho(char *, int, char *);

/*
**++
**  ROUTINE:	Retrieve_Article
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Retrieves an entire article, both headers and body.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Retrieve_Article(int artnum, struct ART **ctxp, char *tsubj, char *xref, char *msgid)
**
** artnum: article number, integer, by value
** ctxp:   pointer to internally-defined structure, modify, by reference
** tsubj:  character string, write only, by reference (ASCIZ)
** xref:   character string, write only, by reference (ASCIZ)
** msgid:  character string, write only, by reference (ASCIZ)
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
unsigned int Retrieve_Article(int artnum, struct ART **ctxp, char *tsubj, char *xref, char *msgid) {

    struct ART *ctx = *ctxp;
    char tmp[STRING_SIZE], *cp;
    struct HDR *hdr;
    int reply_code;
    unsigned int status;

/*
** We cache one entire article, making CURRENT and EXTRACT a little
** faster.  A larger cache may be appropriate for some time in the future.
*/
    ctx = NULL;
    if (artsave != NULL) {
    	if (artnum == artsave->artnum && artsave->grpptr == curgroup) {
    	    ctx = artsave;
    	    artsave = NULL;
    	} else {
    	    wipe_article(&artsave);
    	}
    }

/*
** Establish new article context
*/
    if (ctx == NULL) {
    	ctx = malloc(sizeof(struct ART));
    	memset(ctx, 0, sizeof(struct ART));
    	ctx->artnum = artnum;
    	ctx->grpptr = curgroup;
    }

/*
** Fetch the article headers
*/
    *ctxp = ctx;
    status = Get_Article_Hdrs(curgroup, artnum, &ctx->hdrqptr, 0, &ctx->hlines);
    if (!OK(status)) {
    	wipe_article(ctxp);
    	return status;
    }

/*
** Find the Subject:, Message-ID:, and Xref: headers
*/
    *tsubj = *xref = *msgid = '\0';

    for (hdr = ctx->hdrqptr->head; hdr != (struct HDR *) ctx->hdrqptr;
    	    	    hdr = hdr->flink) {
    	if (hdr->code == NEWS_K_HDR_SUBJECT) {
    	    strcpy(tmp, hdr->str);
    	    upcase(tmp);
    	    if (strncmp(tmp, "RE:", 3) == 0) {
    	    	cp = tmp+3;
    	    } else {
    	    	cp = tmp;
    	    }
    	    while (isspace(*cp)) cp++;
    	    strcpy(tsubj, cp);
    	    for (cp = tsubj+strlen(tsubj); cp > tsubj && isspace(*(cp-1)); cp--);
    	    *cp = '\0';
    	} else if (hdr->code == NEWS_K_HDR_XREF) {
    	    strcpy(xref, hdr->str);
    	} else if (hdr->code == NEWS_K_HDR_REFERENCES) {
    	    strcpy(msgid, hdr->str);
    	} else if (hdr->code == NEWS_K_HDR_MESSAGE_ID && *msgid == '\0') {
    	    strcpy(msgid, hdr->str);
    	}
    }

/*
** Fetch the article body
*/
    if (ctx->bodyfspec[0] == '\0') {
    	make_temp_fspec(tmp, sizeof(tmp));
    	status = Get_Article_Body(artnum, tmp, ctx->bodyfspec, &ctx->lines, 1);
    	if (!OK(status)) {
    	    wipe_article(ctxp);
    	    return status;
    	}
    }

/*
** Set things up for Show_Article_Page
*/
    ctx->bodyunit = 0;
    ctx->status = ART_K_STATUS_RETRIEVED;
    ctx->hlnptr = ctx->hdrqptr->head;
    ctx->remlines = ctx->hlines + ctx->lines + 1;

    return SS$_NORMAL;

} /* Retrieve_Article */

/*
**++
**  ROUTINE:	Show_Article_Page
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Displays the next screen page of an article.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Show_Article_Page(struct ART **ctxp)
**
** ctxp: pointer to internally-defined structure, modify, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	    Normal successful completion.
**  	NEWS__EOARTICLE:    Normal completion; article has been fully displayed.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int Show_Article_Page(struct ART **ctxp) {

    struct ART *ctx = *ctxp;
    struct GRP *grp = (*ctxp)->grpptr;
    struct HDR *hdr;
    struct dsc$descriptor sdsc;
    char tmp[STRING_SIZE];
    short tlen;
    int len;
    unsigned int status;
    static $DESCRIPTOR(alphabet, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    static $DESCRIPTOR(rotabet,  "NOPQRSTUVWXYZABCDEFGHIJKLMnopqrstuvwxyzabcdefghijklm");

/*
** This is done first - just after the article has been retrieved.
*/
    if (ctx->status == ART_K_STATUS_RETRIEVED) {
    	static $DESCRIPTOR(ctrstr, "article !UL of [!UL,!UL] (!UL unseen)");
    	char tmp2[STRING_SIZE], *cp;
    	int i = strlen(grp->grpnam);
    	INIT_SDESC(sdsc, sizeof(tmp), tmp);
    	sys$fao(&ctrstr, &tlen, &sdsc, ctx->artnum, grp->frstavl,
    	    	grp->lastavl, Count_Unseen(grp));
    	*(tmp+tlen) = '\0';
    	if (tlen+i < pb_cols) {
    	    strcpy(tmp2, grp->grpnam);
    	    for (cp = tmp2+i; cp < tmp2+(pb_cols-tlen); cp++) *cp = ' ';
    	    strcpy(cp, tmp);
    	} else strcpy(tmp2, grp->grpnam);
    	Begin_Paged_Output(tmp2);
    	ctx->status = ART_K_STATUS_IN_HEADERS;
    }

/*
** Header display
*/
    if (ctx->status == ART_K_STATUS_IN_HEADERS) {
    	while (ctx->hlnptr != (struct HDR *) ctx->hdrqptr) {
    	    ctx->remlines--;
    	    if (Read_Full_Headers) {
    	    	Format_Header(ctx->hlnptr, tmp, sizeof(tmp), 0);
    	    	if (!Put_Paged(tmp, ctx->remlines)) return SS$_NORMAL;
    	    } else {
    	    	for (hdr = news_prof.hdrlist.head;
    	    	    	hdr != (struct HDR *) &news_prof.hdrlist;
    	    	    	hdr = hdr->flink) {
    	    	    if (hdr->code == ctx->hlnptr->code) {
    	    	    	Format_Header(ctx->hlnptr, tmp, sizeof(tmp), 0);
    	    	    	if (!Put_Paged(tmp, ctx->remlines)) return SS$_NORMAL;
    	    	    	break;
    	    	    }
    	    	}
    	    }
    	    ctx->hlnptr = ctx->hlnptr->flink;
    	}

/*
** Setup for display of article body
*/
    	status = file_open(ctx->bodyfspec, &ctx->bodyunit, 0, 0, 0);
    	if (!OK(status)) {
    	    lib$signal(NEWS__BODYERR, 2, strlen(ctx->bodyfspec),
    	    	    	    ctx->bodyfspec, status);
    	    ctx->bodyunit = 0;
    	    ctx->status = ART_K_STATUS_DONE;
    	} else {
    	    ctx->status = ART_K_STATUS_IN_BODY;
    	    ctx->remlines--;
    	    if (!Put_Paged("", ctx->remlines)) return SS$_NORMAL;
    	}
    }

/*
** Display of article body
*/
    if (ctx->status == ART_K_STATUS_IN_BODY) {
    	while (OK(file_read(ctx->bodyunit, tmp, sizeof(tmp)-1, &len))) {
    	    ctx->remlines--;
    	    *(tmp+len) = '\0';
    	    if (rotate_text) {
    	    	struct dsc$descriptor tdsc;
    	    	char tmp2[STRING_SIZE];
    	    	INIT_SDESC(sdsc, len, tmp);
    	    	INIT_SDESC(tdsc, len, tmp2);
    	    	*(tmp2+len) = '\0';
    	    	str$translate(&tdsc, &sdsc, &alphabet, &rotabet);
    	    	
    	    	if (!Put_Paged(tmp2, ctx->remlines)) return SS$_NORMAL;
    	    } else if (!Put_Paged(tmp, ctx->remlines)) return SS$_NORMAL;
    	}

    	ctx->status = ART_K_STATUS_DONE;

    }

/*
** Completion
*/

    if (Paged_Output_Done()) {
    	if (ctx->bodyunit != 0) file_close(ctx->bodyunit);
    	ctx->bodyunit = 0;
    	artsave = ctx;
    	return NEWS__EOARTICLE;
    }

    return SS$_NORMAL;

} /* Show_Article_Page */

/*
**++
**  ROUTINE:	Rewind_Article
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Sets up an article for redisplay from the beginning.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Rewind_Article(struct ART **ctxp)
**
** ctxp: pointer to internally-defined structure, modify, by reference
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
void Rewind_Article(struct ART **ctxp) {

    struct ART *ctx = *ctxp;

    Discard_Paged_Output();
    file_setpos(ctx->bodyunit, 0);
    ctx->hlnptr = ctx->hdrqptr->head;
    ctx->status = ART_K_STATUS_IN_HEADERS;

} /* Rewind_Article */

/*
**++
**  ROUTINE:	Close_Article
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Closes an article, but saves the context in case we
**  need it again right away.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Close_Article(struct ART **ctxp)
**
** ctxp: pointer to internally-defined structure, modify, by reference
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
void Close_Article(struct ART **ctxp) {

    struct ART *ctx = *ctxp;

    Discard_Paged_Output();
    if (ctx->bodyunit != 0) file_close(ctx->bodyunit);
    ctx->bodyunit = 0;
    artsave = ctx;
    *ctxp = NULL;

} /* Close_Article */

/*
**++
**  ROUTINE:	wipe_article
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Deletes an article and its context.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	wipe_article(struct ART **ctxp)
**
** ctxp: pointer to internally-defined structure, modify, by reference
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
static void wipe_article(struct ART **ctxp) {

    struct ART *ctx = *ctxp;

    if (ctx->bodyfspec[0]) {
    	file_delete(ctx->bodyfspec);
    }
    free(ctx);
    *ctxp = NULL;

} /* wipe_article */

/*
**++
**  ROUTINE:	Mark_Article_Seen
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Adds an article to a group's seen list.
**
**  RETURNS:	boolean
**
**  PROTOTYPE:
**
**  	Mark_Article_Seen(struct GRP *grp, int artnum)
**
** grp:     GRP structure, modify, by reference
** artnum:  article number, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	1:  Success.  Article marked seen.
**  	0:  Article was already marked.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int Mark_Article_Seen(struct GRP *grp, int artnum) {

    struct RNG *r, *r2;

/*
** The seen list is represented as a linked list of cells, each of which
** contains a starting and ending article number for a range of seen articles.
** The list is kept in order and cells are merged together into one when
** they overlap.
**
** Start by finding the right cell for this article number.
*/
    for (r = grp->seenq.head; r != (struct RNG *) &grp->seenq; r = r->flink) {
    	if (artnum <= r->last) break;
    }
/*
** If we found none, artnum must be higher than all articles seen so far.
** So we tack it onto the tail of the list.
*/
    if (r == (struct RNG *) &grp->seenq) {
    	r = grp->seenq.tail;
    	if (r != (struct RNG *) &grp->seenq) {
    	    if (artnum == r->last+1) {     /* no new cell if just 1 higher */
    	    	r->last += 1;                                               
    	    } else {
    	    	r2 = mem_getrng();
    	    	r2->first = r2->last = artnum;
    	    	queue_insert(r2, r);
    	    }
    	} else {                     /* list was empty */
    	    r = mem_getrng();
    	    r->first = r->last = artnum;
    	    queue_insert(r, &grp->seenq);
    	}
/*
** We found a cell with article numbers higher than the current article.
** Now we check to see if we need a new cell for this article, or whether
** we can just update the existing cell or its predecessor in the list.
*/
    } else {
    	if (artnum >= r->first) return 0; /* already marked */
    	r2 = r->blink;
    	if (artnum == r->first-1) { 	  /* Just update this cell */
    	    r->first -= 1;
    	    if (r2 != (struct RNG *) &grp->seenq) {  /* See if we now overlap */
    	    	if (r2->last >= r->first-1) {        /* with predecessor; if  */
    	    	    r->first = r2->first;            /* so, coalesce the two  */
    	    	    queue_remove(r2, &r2);           /* cells into one.       */
    	    	    mem_freerng(r2);
    	    	}
    	    }
    	} else {
    	    if (r2 != (struct RNG *) &grp->seenq) {
    	    	if (artnum == r2->last+1) { 	    /* tack onto end of   */
    	    	    r2->last += 1;  	    	    /* predecessor cell   */
    	    	} else {
    	    	    r2 = mem_getrng();	    	    /* New cell required  */
    	    	    r2->first = r2->last = artnum;
    	    	    queue_insert(r2, r->blink);
    	    	}
    	    } else {
    	    	r2 = mem_getrng();  	    	    /* no previous; get new  */
    	    	r2->first = r2->last = artnum;	    /* and stick at listhead */
    	    	queue_insert(r2, &grp->seenq);
    	    }
    	}
    }

    return 1;

} /* Mark_Article_Seen */

/*
**++
**  ROUTINE:	Mark_Article_Unseen
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Removes an article from the seen-article list for a group.
**
**  RETURNS:	boolean
**
**  PROTOTYPE:
**
**  	Mark_Article_Unseen(struct GRP *grp, int artnum)
**
** grp:	    GRP structure, modify, by reference
** artnum:  article number, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    1:	success
**  	    0:	Article was not on seen list
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int Mark_Article_Unseen(struct GRP *grp, int artnum) {

    struct RNG *r, *r2;

/*
** Locate the cell that this article number is a part of
*/
    for (r = grp->seenq.head; r != (struct RNG *) &grp->seenq; r = r->flink) {
    	if (artnum <= r->last) break;
    }

    if (r == (struct RNG *) &grp->seenq) return 0;  /* none found */
    if (artnum < r->first) return 0;	    	    /* between two cells */

/*
** If number is the boundary of the cell, just update the cell boundary,
** and check to see if the cell should go away.
*/
    if (artnum == r->first || artnum == r->last) {
    	if (artnum == r->first) r->first++;
    	else r->last--;
    	if (r->first > r->last) {
    	    queue_remove(r, &r);
    	    mem_freerng(r);
    	}
    	return 1;
    }

/*
** Must break the cell up into two pieces.
*/
    r2 = mem_getrng();
    r2->first = artnum+1;
    r2->last = r->last;
    r->last = artnum-1;
    queue_insert(r2, r);

    return 1;

} /* Mark_Article_Unseen */

/*
**++
**  ROUTINE:	Mark_Xref
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Parse an Xref header and marks the referenced articles [un]seen.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Mark_Xref(char *xref, int unsee)
**
** xref: character string, read only, by reference (ASCIZ)
** unsee: boolean, read only, by value
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
void Mark_Xref(char *xref, int unsee) {

    struct QUE grpq;
    struct HDR *h;
    struct GRP *g;

    grpq.head = grpq.tail = &grpq;
    Parse_Xref(xref, &grpq);
    while (queue_remove(grpq.head, &h)) {
    	g = Find_Group(h->str);
    	if (g != NULL) {
    	    if (unsee) {
    	    	Mark_Article_Unseen(g, h->code);
    	    } else {
    	    	Mark_Article_Seen(g, h->code);
    	    }
    	}
    	mem_freehdr(h);
    }

} /* Mark_Xref */

/*
**++
**  ROUTINE:	Next_Unseen_Article
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Finds the next unseen article in a group (or in the current group).
**
**  RETURNS:	article number
**
**  PROTOTYPE:
**
**  	Next_Unseen_Article(int artnum, int nocheck, struct GRP *grp)
**
** artnum:  article number, read only, by value
** nocheck: boolean, read only, by value
** grp:	    GRP structure, read only, by reference
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	non-0:	next unseen article
**  	    0:  no unseen articles left
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int Next_Unseen_Article(int artnum, int nocheck, struct GRP *grp) {

    struct RNG *r;
    int a;

    if (grp == NULL) grp = curgroup;
    a = (grp->frstavl > artnum + 1) ? grp->frstavl : artnum+1;
    if (artnum > grp->lastavl) return 0;

    while (1) {

/*
** Check to see if it's on the seen list
*/
    	for (r = grp->seenq.tail; r != (struct RNG *) &grp->seenq; r = r->blink) {
    	    if (a >= r->first) break;
    	}
    	if (r != (struct RNG *) &grp->seenq) {
    	    if (a <= r->last) {
    	    	a = r->last+1;
    	    	if (a > grp->lastavl) {
    	    	    a = 0;
    	    	    break;
    	    	}
    	    }
    	}
    	if (a > grp->lastavl) {
    	    a = 0;
    	    break;
    	}
/*
** Normally, we STAT the article to see if it actually exists on the
** server, but we don't have to if XHDR is used (different mechanism
** for finding holes).
*/
    	if (!nocheck) {

    	    static $DESCRIPTOR(ctrstr, "STAT !UL");
    	    struct dsc$descriptor sdsc;
    	    char tmp[STRING_SIZE];
    	    short tlen;
    	    int reply_code, i;

    	    i = Is_Valid_Article(curgroup, a);
    	    if (i > 0) break;
    	    else if (i < 0) {
    	    	INIT_SDESC(sdsc, sizeof(tmp)-1, tmp);
    	    	sys$fao(&ctrstr, &tlen, &sdsc, a);
    	    	*(tmp+tlen) = '\0';
    	    	server_send(tmp);
    	    	server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);
    	    	if (reply_code == NNTP__TEXTSEPARATE) {
    	    	    if (grp->valid != 0)
    	    	    	grp->valid[a-grp->frstavl] = GRP_K_VALID;
    	    	    break;
    	    	} else {
    	    	    Mark_Article_Seen(curgroup, a);
    	    	    if (grp->valid != 0)
    	    	    	grp->valid[a-grp->frstavl] = GRP_K_INVALID;
    	    	}
    	    }
    	} else break;
    	a += 1;
    }

    return a;

} /* Next_Unseen_Article */

/*
**++
**  ROUTINE:	Count_Unseen
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Counts the number of unseen articles in a group.
**
**  RETURNS:	int (article count)
**
**  PROTOTYPE:
**
**  	Count_Unseen(struct GRP *g)
**
** g:	GRP structure, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	-1: 	something is very wrong here!
**  	not -1: that's the number of unseen articles
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int Count_Unseen(struct GRP *g) {

    struct RNG *r, *r2;
    int s, a, b, count;

    s = g->frstavl-1;
    if (s > g->lastavl) s = g->lastavl;
    if (g->lastavl == 0 && g->frstavl == 0) return 0;
    r = g->seenq.head;
    if (r == (struct RNG *) &g->seenq) {
    	return (g->lastavl-s > 0) ? g->lastavl-s : 0;
    }

    count = 0;
    while (1) {
    	r2 = r->flink;
    	a = r->last > s ? r->last : s;
    	if (a > g->lastavl) a = g->lastavl;
    	if (r2 == (struct RNG *) &g->seenq) {
    	    return count+g->lastavl-a;
    	}
    	b = r2->first > s ? r2->first : s;
    	if (b > g->lastavl) b = g->lastavl;
    	count = count + (b-a > 1 ? b-a-1 : 0);
    	r = r2;
    }

    return -1;  /* should never reach here */

} /* Count_Unseen */

/*
**++
**  ROUTINE:	Post_Article
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Sends an article to the NNTP server for posting.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Post_Article(struct QUE *hdrq, char *fspec, int use_signature)
**
** hdrq:    QUE structure, modify, by reference
** fspec:   file specification, read only, by reference (ASCIZ string)
** use_signature: boolean, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Article was successfully posted.
**  	NEWS__POSTERR:	Some error occurred.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int Post_Article(struct QUE *hdrq, char *fspec, int use_signature) {

    char tmp[STRING_SIZE];
    struct dsc$descriptor sdsc;
    struct HDR *hdr;
    TIME tod;
    unsigned int unit, status;
    int reply_code, len;
    short tlen;

/*
** Open the article body file (if there is one)
*/
    if (fspec) {
    	status = file_open(fspec, &unit, 0, 0, 0);
    	if (!OK(status)) {
    	    lib$signal(NEWS__POSTERR, 0, status);
    	    return NEWS__POSTERR;
    	}
    }

/*
** Prepare the server for the onslaught
*/
    server_send("POST");
    server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);

/*
**  Authentication stuff
*/
    if (reply_code == NNTP__AUTHREQD) {

    	put_output("Authentication required for posting.");

    	strcpy(tmp, "AUTHINFO USER ");
    	get_cmd(tmp+14, sizeof(tmp)-14, "Username: ");
    	server_send(tmp);
    	server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);

    	if (reply_code == NNTP__PASSWREQD) {
    	    strcpy(tmp, "AUTHINFO PASS ");
    	    get_cmd_noecho(tmp+14, sizeof(tmp)-14, "Password: ");
    	    server_send(tmp);
    	    server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);
    	}

    	if (reply_code != NNTP__AUTHACCEPTED) {
    	    lib$signal(NEWS__NOPOSTING, 0);
    	    return NEWS__NOPOSTING;
    	}

    	server_send("POST");
    	server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);

    }	    
    
    if (reply_code != NNTP__SENDPARTICLE) {
    	lib$signal(NEWS__NOPOSTING, 0);
    	return NEWS__NOPOSTING;
    }

/*
** Get current date and time for the couple of headers we generate
*/
    sys$gettim(&tod);

/*
** Format the headers and send them.  Note that this destroys the
** header queue passed to us!
*/
    while (queue_remove(hdrq->head, &hdr)) {
    	Format_Header(hdr, tmp, sizeof(tmp), 0);
    	server_send(tmp);
    	mem_freehdr(hdr);
    }

/*
** Add the standard headers.  The news system usually sorts the headers
** into some standard arrangement, so we really don't have to worry about
** how pretty this order looks.
*/
    strcpy(tmp, "From: ");
    Make_Return_Address(tmp+6, sizeof(tmp)-6);
    server_send(tmp);
    strcpy(tmp, "Reply-To: ");
    strcat(tmp, news_cfg.reply_to);
    server_send(tmp);

    INIT_SDESC(sdsc, sizeof(tmp)-1, tmp);
    if (news_cfg.dopath) {
    	if (news_cfg.pathstr[0] == '\0') {
    	    if (news_cfg.bangpath) {
    	    	static $DESCRIPTOR(ctrstr, "Path: !AD!!!AD");
    	    	sys$fao(&ctrstr, &tlen, &sdsc,
    	    	    strlen(news_cfg.node_name), news_cfg.node_name,
    	    	    strlen(news_cfg.username), news_cfg.username);
    	    	*(tmp+tlen) = '\0';
    	    } else {
    	    	static $DESCRIPTOR(ctrstr, "Path: !AD@!AD");
    	    	sys$fao(&ctrstr, &tlen, &sdsc, strlen (news_cfg.username),
    	    	    news_cfg.username, strlen(news_cfg.node_name),
    	    	    news_cfg.node_name);
    	    	*(tmp+tlen) = '\0';
    	    }
    	} else {
    	    strcpy(tmp, "Path: ");
    	    strcat(tmp, news_cfg.pathstr);
    	    strcat(tmp, news_cfg.username);
    	}
    	server_send(tmp);
    }

    if (news_cfg.gendate) {
    	strcpy(tmp, "Date: ");
    	Make_Date(&tod, tmp+6, sizeof(tmp)-6);
    	server_send(tmp);
    }
    if (news_cfg.genmsgid) {
    	static $DESCRIPTOR(ctrstr, "Message-ID: <!XL.!XL@!AD>");
    	sys$fao(&ctrstr, &tlen, &sdsc, tod.long2, tod.long1,
    	    	    strlen(news_cfg.node_name), news_cfg.node_name);
    	*(tmp+tlen) = '\0';
    	server_send(tmp);
    }
    if (news_cfg.org_name[0] != '\0') {
    	strcpy(tmp, "Organization: ");
    	strcat(tmp, news_cfg.org_name);
    	server_send(tmp);
    }

/*
** End of headers
**/
    server_send("");

/*
** Now send the body, if there is one.
*/
    if (fspec) {
    	char *cp;
    	tmp[0] = '.';
    	while (OK(file_read(unit, tmp+1, sizeof(tmp)-2, &len))) {
    	    *(tmp+1+len) = '\0';
    	    cp = (*(tmp+1) == '.') ? tmp : tmp+1;
    	    if (news_cfg.chrcnv) {
    	    	char tmp2[STRING_SIZE*2];
    	    	(*news_cfg.chrlton)(cp, tmp2, sizeof(tmp2));
    	     	server_send(tmp2);
    	    } else {
    	    	server_send(cp);
    	    }
    	}

    	file_close(unit);
    	unit = 0;

    }

/*
** Attach signature, if we're supposed to.
*/

    if (use_signature && news_prof.sigfile[0] != '\0') {
    	status = file_open(news_prof.sigfile, &unit, 0, 0, 0);
    	if (!OK(status)) {
    	    lib$signal(NEWS__SIGFERR, 2, strlen(news_prof.sigfile),
    	    	    	    news_prof.sigfile, status);
    	} else {
    	    int i;
    	    char *cp;
    	    tmp[0] = '.';
    	    for (i = 0; i < 8; i++) {
    	    	if (!OK(file_read(unit, tmp+1, sizeof(tmp)-2, &len))) break;
    	    	*(tmp+1+len) = '\0';
    	    	cp = (*(tmp+1) == '.') ? tmp : tmp+1;
    	    	if (news_cfg.chrcnv) {
    	    	    char tmp2[STRING_SIZE*2];
    	    	    (*news_cfg.chrlton)(cp, tmp2, sizeof(tmp2));
    	     	    server_send(tmp2);
    	    	} else {
    	       		server_send(cp);
    	    	}
    	    }
    	    file_close(unit);
    	    unit = 0;
    	}
    }

/*
** End of the article.  Echo the reply from the server.
*/
    server_send(".");
    server_get_reply(SRV__ECHO, &reply_code, 0, 0, 0);
    if (reply_code != NNTP__ARTICLEPOSTED) {
    	lib$signal(NEWS__POSTERR, 0);
    	return NEWS__POSTERR;
    }

    return SS$_NORMAL;

} /* Post_Article */

/*
**++
**  ROUTINE:	Get_Article_Hdrs
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Retrieves an article's headers, either from the local
**  header cache or from the server.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Get_Article_Hdrs(struct GRP *grp, int artnum, struct QUE **hdrqp,
**  	    	    	    	    int nosignal, int *lines)
** grp:	     GRP structure, read only, by reference
** artnum:   article number, read only, by value
** hdrqp:    pointer to QUE structure, write only, by reference
** nosignal: boolean, read only, by value
** lines:    integer, write only, by reference
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
unsigned int Get_Article_Hdrs(struct GRP *grp, int artnum, struct QUE **hdrqp,
    	    	    	    	    int nosignal, int *lines) {

    struct QUE tmpq;
    struct CHDR *ch, *ch2;
    struct HDR *hdr, *hdr2;
    char tmp[STRING_SIZE], tmp2[STRING_SIZE];
    char *cp;
    int reply_code, len;
    unsigned int status;

/*
** If the requested group is the group we're caching, then search the
** cache for the headers.
*/
    if (hcgroup == grp) {
    	ch = Cache_Lookup(artnum);
    	if (ch != 0 && artnum == ch->artnum) {
    	    if (ch->have_all_headers && ch->hdrq.head == &(ch->hdrq)) {
    	    	if (!nosignal) {
    	    	    lib$signal(NEWS__ARTRERR, 3, strlen(grp->grpnam),
    	    	    	    	    grp->grpnam, artnum,
    	    	    	       NEWS__NOSUCHARTICLE, 0);
    	    	}
    	    	return NEWS__NOSUCHARTICLE;
    	    }
    	    if (ch->have_all_headers) {
    	    	*hdrqp = &(ch->hdrq);
    	    	if (lines) *lines = ch->hlines;
    	    	return SS$_NORMAL;
    	    }
    	}
/*
** New group, so wipe the cache and start anew.
*/
    } else {
    	Cache_Init(grp);
    	ch = 0;
    }

/*
** If the request is for a group other than the current group,
** coordinate with the server.
*/
    if (grp != curgroup) {
    	strcpy(tmp, "GROUP ");
    	strcat(tmp, grp->grpnam);
    	server_send(tmp);
    	server_get_reply(SRV__NOECHO, &reply_code, tmp, sizeof(tmp), &len);
    	if (reply_code != NNTP__GRPSELECTED) {
    	    if (!nosignal) lib$signal(NEWS__ARTRERR, 3, strlen(grp->grpnam),
    	    	    grp->grpnam, artnum, NEWS__UNEXPRSP, 3,
    	    	    reply_code, len, tmp);
    	    return NEWS__ARTRERR;
    	}
    }
/*
** Get the headers from the server
*/
    sprintf(tmp, "HEAD %d", artnum);
    server_send(tmp);
    server_get_reply(SRV__NOECHO, &reply_code, tmp, sizeof(tmp)-1, &len);
    tmp[len] = '\0';
    if (reply_code != NNTP__HEADFOLLOWS) {
    	unsigned int status;
    	if (reply_code == NNTP__NOSUCHARTNUM || reply_code == NNTP__NOSUCHARTICLE) {
    	    if (grp->valid != 0) grp->valid[artnum-grp->frstavl] = GRP_K_INVALID;
    	    if (!nosignal) lib$signal(NEWS__ARTRERR, 3, strlen(grp->grpnam),
    	    	    	    	grp->grpnam, artnum, NEWS__NOSUCHARTICLE, 0);
    	    status = NEWS__NOSUCHARTICLE;
    	} else {
    	    if (!nosignal) {
    	    	lib$signal(NEWS__ARTRERR, 3, strlen(grp->grpnam),
    	    	    grp->grpnam, artnum, NEWS__UNEXPRSP,
    	    	    3, reply_code, len, tmp);
    	    }
    	    status = NEWS__ARTRERR;
    	}
    	if (grp != curgroup) {
    	    strcpy(tmp, "GROUP ");
    	    strcat(tmp, curgroup->grpnam);
    	    server_send(tmp);
    	    server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);
    	}
    	return status;
    }
    if (grp->valid != 0) grp->valid[artnum-grp->frstavl] = GRP_K_VALID;

/*
** This loop reads the lines coming from the server and converts them
** into headers.  Servers don't usually wrap lines, but this code handles
** that case.
*/
    tmpq.head = tmpq.tail = &tmpq;
    cp = tmp2;
    tmp2[0] = '\0';
    while (server_get_line(tmp, sizeof(tmp)-1, &len) != NEWS__EOLIST) {
    	struct HDR *hln;
    	while (len > 0 && isspace(tmp[len-1])) len--;
    	*(tmp+len) = '\0';
    	if (isspace(*tmp)) {
    	    if (len > sizeof(tmp2)-(cp-tmp2)) len = sizeof(tmp2)-(cp-tmp2);
    	    if (len > 0) memcpy(cp, tmp, len);
    	    cp += len;
    	    *cp = '\0';
    	} else {
    	    int tlen;
    	    if (tmp2[0]) {
    	    	tlen = strlen(tmp2);
    	    	hln = mem_gethdr(tlen+1);
    	    	hln->len = tlen;
    	    	strcpy(hln->str, tmp2);
    	    	queue_insert(hln, tmpq.tail);
    	    }
    	    if (len > sizeof(tmp2)-1) len = sizeof(tmp2)-1;
    	    memcpy(tmp2, tmp, len);
    	    cp = tmp2+len;
    	    *cp = '\0';
    	}
    }

    if (tmp2[0]) {
    	struct HDR *hln;
    	int tlen;
    	tlen = strlen(tmp2);
    	hln = mem_gethdr(tlen+1);
    	hln->len = tlen;
    	strcpy(hln->str, tmp2);
    	queue_insert(hln, tmpq.tail);
    }

    ch2 = Cache_Add(ch, artnum, 1);
    ch2->have_all_headers = 1;

/*
** Parse the headers, sticking the result in the cache, then wipe
** the original text version.  Set hdrqp to point to the cached headers.
*/
    Parse_Headers(&tmpq, &(ch2->hdrq), &(ch2->hlines));
    while (queue_remove(tmpq.head, &hdr)) mem_freehdr(hdr);

/*
**  Set up the quick-access pointers
*/
    for (hdr = ch2->hdrq.head, len = 0; len != XHDR_MASK &&
    	    	hdr != (struct HDR *) &(ch2->hdrq); hdr = hdr->flink) {
    	int i;

    	for (i = 0; i < XHDR_COUNT; i++) {
    	    if (hdr->code == xhdr_codes[i]) {
    	    	ch2->xhdrs[i] = hdr;
    	    	len |= (1<<i);
    	    	break;
    	    }
    	}
    }

    *hdrqp = &(ch2->hdrq);
    if (lines) *lines = ch2->hlines;

/*
** If we had changed the group on the server, change back again
*/
    if (grp != curgroup) {
    	strcpy(tmp, "GROUP ");
    	strcat(tmp, curgroup->grpnam);
    	server_send(tmp);
    	server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);
    }

    return SS$_NORMAL;

} /* Get_Article_Hdrs */

/*
**++
**  ROUTINE:	Get_One_Header
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Retrieves a single header for an article, either from the local
**  header cache or from the server.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Get_One_Header(struct GRP *grp, int artnum, int code, struct HDR **hdr)
**
** grp:	     GRP structure, read only, by reference
** artnum:   article number, read only, by value
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
unsigned int Get_One_Header(struct GRP *grp, int artnum, int code,
    	    	    	    	    	    	    struct HDR **hdrp) {

    struct QUE tmpq;
    struct CHDR *ch, *ch2;
    struct HDR *hdr, *hdr2;
    char tmp[STRING_SIZE], tmp2[STRING_SIZE];
    char *cp;
    int reply_code, len, i, is_xover_hdr, fallback;
    unsigned int status;
    static char *hdrname[] = {
    	"From", "Date", "Newsgroups", "Subject", "Message-ID",
    	"Path", "Reply-To", "Sender", "Followup-To", "Expires",
    	"References", "Control", "Distribution", "Organization",
    	"Keywords", "Summary", "Approved", "Lines", "Xref", "NoSuchHeader..."};

/*
** If the requested group is the group we're caching, then search the
** cache for the headers.
*/
    if (hcgroup == grp) {
    	ch = Cache_Lookup(artnum);
    } else {
    	Cache_Init(grp);
    	ch = 0;
    }

/*
**  Is it in the cache? 
*/
    if (ch != 0 && ch->artnum == artnum) {
    	for (i = 0; i < XHDR_COUNT; i++) {
    	    if (code == xhdr_codes[i]) {
    	    	if (ch->have_all_headers || (ch->have_xhdrs & (1<<i))) {
    	    	    *hdrp = ch->xhdrs[i];
    	    	    return SS$_NORMAL;
    	    	}
    	    }
    	}
    	*hdrp = 0;
    	for (hdr = ch->hdrq.head; hdr != (struct HDR *) &ch->hdrq; hdr = hdr->flink) {
    	    if (hdr->code == code) {
    	    	*hdrp = hdr;
    	    	return SS$_NORMAL;
    	    }
    	}
    }

/*
**  Not in the cache.  We're going to have to talk to the server.
**  First see if this is one of the headers returned by XOVER.
*/

    fallback = is_xover_hdr = 0;
    for (i = 0; i < sizeof(xover_hdrs)/sizeof(xover_hdrs[0]); i++) {
    	if (xover_hdrs[i] < 0) continue;
    	if (code == xhdr_codes[xover_hdrs[i]]) {
    	    is_xover_hdr = 1;
    	    break;
    	}
    }
/*
**  If XOVER and XHDR aren't supported on the server, do a full header fetch
*/

Do_Full_Fetch:
    if (news_cfg.xhdr < 0 && (!is_xover_hdr || news_cfg.xover < 0)) {
    	struct QUE *hdrq;
    	status = Get_Article_Hdrs(grp, artnum, &hdrq, 1, 0);
    	if (OK(status)) {
    	    *hdrp = 0;
    	    for (hdr = hdrq->head; hdr != (struct HDR *) hdrq; hdr = hdr->flink) {
    	    	if (hdr->code == code) {
    	    	    *hdrp = hdr;
    	    	    break;
    	    	}
    	    }
    	}
    	return status;
    }

/*
** If the request is for a group other than the current group,
** coordinate with the server.
*/
    if (grp != curgroup) {
    	strcpy(tmp, "GROUP ");
    	strcat(tmp, grp->grpnam);
    	server_send(tmp);
    	server_get_reply(SRV__NOECHO, &reply_code, tmp, sizeof(tmp), &len);
    	if (reply_code != NNTP__GRPSELECTED) return NEWS__ARTRERR;
    }

/*
** Try XOVER first, if this is an XOVER header.
*/
    hdr = 0;
    if (is_xover_hdr && news_cfg.xover >= 0) {
    	sprintf(tmp, "XOVER %d", artnum);
    	server_send(tmp);
    	server_get_reply(SRV__NOECHO, &reply_code, tmp, sizeof(tmp)-1, &len);
    	tmp[len] = '\0';
/*
**  If we get back a DATAFOLLOWS reply, then we parse out the XOVER header
**  response and get back a lot more information that might come in handy
**  later.
*/
    	if (reply_code == NNTP__DATAFOLLOWS) {
    	    int did_it, j, n;
    	    char *cp, *anchor;
    	    struct HDR *h, *h2;

    	    did_it = 0;
    	    news_cfg.xover = 1;
    	    while (server_get_line(tmp, sizeof(tmp)-1, &len) != NEWS__EOLIST) {
    	    	int i;
    	    	tmp[len] = '\0';
    	    	cp = strchr(tmp, '\t');
    	    	if (cp == 0) {
    	    	    cp = tmp + len;
    	    	    fallback = 1;
    	    	}
    	    	*cp = '\0';
    	    	n = atoi(tmp);
    	    	if (n != artnum) continue;
    	    	if (grp->valid != 0) grp->valid[n-grp->frstavl] = GRP_K_VALID;
    	    	anchor = cp + 1;
    	    	did_it = 1;
/*
**  This is the article we want.  Let's add it to the cache
*/
    	    	ch2 = Cache_Add(ch, artnum, 0);
/*
**  Now let's get the headers from the response.
*/
    	    	for (i = 0; anchor < (tmp + len) &&
    	    	    	    i < sizeof(xover_hdrs)/sizeof(xover_hdrs[0]); i++) {
    	    	    cp = strchr(anchor, '\t');
    	    	    if (cp == 0) cp = tmp + len;
    	    	    if (xover_hdrs[i] < 0) {
    	    	    	anchor = cp + 1;
    	    	    	continue;
    	    	    }
    	    	    if (cp == anchor) {
    	    	    	ch2->xhdrs[xover_hdrs[i]] = 0;
    	    	    	ch2->have_xhdrs |= (1<<xover_hdrs[i]);
    	    	    	anchor = cp + 1;
    	    	    	continue;
    	    	    }
    	    	    *cp = '\0';
    	    	    j = strlen(anchor);
    	    	    h = mem_gethdr(j);
    	    	    strcpy(h->str, anchor);
    	    	    h->code = xhdr_codes[xover_hdrs[i]];
    	    	    h->len = j;
    	    	    if (h->code == code) hdr = h;
    	    	    for (h2 = ch2->hdrq.head; h2 != (struct HDR *) &ch2->hdrq;
    	    	    	    h2 = h2->flink) {
    	    	    	if (h2->code == h->code) break;
    	    	    }
    	    	    if (h2 == (struct HDR *) &ch2->hdrq) {
    	    	    	queue_insert(h, ch2->hdrq.tail);
    	    	    	ch2->xhdrs[xover_hdrs[i]] = h;
    	    	    	ch2->have_xhdrs |= (1<<xover_hdrs[i]);
    	    	    } else mem_freehdr(h);
    	    	    anchor = cp + 1;
    	    	}  /* loop through the XOVER response fields */
    	    } /* loop through the XOVER response lines */
/*
**  If we didn't see the article we wanted, then it doesn't exist on the
**  server.
*/
    	    if (!did_it) {
    	    	if (grp->valid != 0)
    	    	    grp->valid[artnum-grp->frstavl] = GRP_K_INVALID;
    	    	if (grp != curgroup) {
    	    	    int i;
    	    	    strcpy(tmp, "GROUP ");
    	    	    strcat(tmp, curgroup->grpnam);
    	    	    server_send(tmp);
    	    	    server_get_reply(SRV__NOECHO, &i, 0, 0, 0);
    	    	}
    	    	return NEWS__ARTRERR;
    	    }
/*
**  If we got back CMDUNRECOGNZD, then this server definitely doesn't
**  support XOVER.
*/
    	} else if (reply_code == NNTP__CMDUNRECOGNZD) {
    	    news_cfg.xover = -1;
    	    if (news_cfg.xhdr < 0) goto Do_Full_Fetch;
    	}
    }
/*
** Try XHDR next, if we have to.  Format the XHDR request...
*/
    if (hdr == 0 && (fallback || !(is_xover_hdr && news_cfg.xover > 0))) {
    	sprintf(tmp, "XHDR %s %d", hdrname[code-1], artnum);
    	server_send(tmp);
    	server_get_reply(SRV__NOECHO, &reply_code, tmp, sizeof(tmp)-1, &len);
    	tmp[len] = '\0';
    	if (reply_code != NNTP__HEADFOLLOWS) {
    	    if (grp != curgroup) {
    	    	int i;
    	    	strcpy(tmp, "GROUP ");
    	    	strcat(tmp, curgroup->grpnam);
    	    	server_send(tmp);
    	    	server_get_reply(SRV__NOECHO, &i, 0, 0, 0);
    	    }
/*
**  If the server doesn't do XHDR, fall back to full header fetch
*/
    	    if (reply_code == NNTP__CMDUNRECOGNZD) {
    	    	news_cfg.xhdr = -1;
    	    	goto Do_Full_Fetch;
    	    }
    	    return NEWS__ARTRERR;
    	} else news_cfg.xhdr = 1;

/*
** Get the header we've requested
*/
    	while (server_get_line(tmp, sizeof(tmp)-1, &len) != NEWS__EOLIST) {
    	    int i = 0;
    	    tmp[len] = '\0';
    	    for (cp = tmp; *cp != '\0' && !isspace(*cp); cp++) i = i * 10 + (*cp - '0');
    	    if (grp->valid != 0) grp->valid[i-grp->frstavl] = GRP_K_VALID;
    	    if (hdr == 0 && i == artnum) {
    	    	while (isspace(*cp)) cp++;
    	    	hdr = mem_gethdr(len - (cp - tmp) + 1);
    	    	strcpy(hdr->str, cp);
    	    	hdr->code = code;
    	    	hdr->len = len - (cp-tmp);
    	    }
    	}
/*
**  If it's a nonexistent article, let 'em know
*/
    	if (hdr == 0) {
    	    if (grp->valid != 0) grp->valid[artnum-grp->frstavl] = GRP_K_INVALID;
    	    if (grp != curgroup) {
    	    	strcpy(tmp, "GROUP ");
    	    	strcat(tmp, curgroup->grpnam);
    	    	server_send(tmp);
    	    	server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);
    	    }
    	    return NEWS__ARTRERR;
    	}

/*
**  XHDR returns (none) if the article exists but the header requested
**  is not present.
*/

    	if (strcmp(hdr->str, "(none)") == 0) {
    	    mem_freehdr(hdr);
    	    hdr = 0;
    	}
/*
** Add this header to the cache.
*/
    	ch2 = Cache_Add(ch, artnum, 0);

    	if (hdr != 0) queue_insert(hdr, ch2->hdrq.tail);
/*
**  Set up the quick-access pointers
*/
    	for (i = 0; i < XHDR_COUNT; i++) {
    	    if (code == xhdr_codes[i]) {
    	    	ch2->xhdrs[i] = hdr;
    	    	ch2->have_xhdrs |= (1<<i);
    	    	break;
    	    }
    	}

    } /* Using XHDR */

    *hdrp = hdr;

/*
** If we had changed the group on the server, change back again
*/
    if (grp != curgroup) {
    	strcpy(tmp, "GROUP ");
    	strcat(tmp, curgroup->grpnam);
    	server_send(tmp);
    	server_get_reply(SRV__NOECHO, &reply_code, 0, 0, 0);
    }

    return SS$_NORMAL;

} /* Get_One_Header */

/*
**++
**  ROUTINE:	Preload_Cache
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Uses XOVER to pre-load the header cache with a range of articles.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Preload_Cache(struct GRP *grp, int artnum, int code, struct HDR **hdr)
**
** grp:	     GRP structure, read only, by reference
** artnum:   article number, read only, by value
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
int Preload_Cache(struct GRP *grp, int first, int last) {

    struct CHDR *ch, *ch2;
    struct HDR *h, *hdr2, *h2;
    char tmp[STRING_SIZE];
    char *cp, *anchor;
    int reply_code, len, i, j, artnum, xi, last_valid, old_min;
    unsigned int status;

/*
**  If this isn't the group we're caching, or we don't have XOVER,
**  forget it.
*/
    if (hcgroup == 0) {
    	Cache_Init(grp);
    	ch = 0;
    }

    if (hcgroup != grp || news_cfg.xover < 0) return 0;

/*
**  Only check the articles we need to for holechecks
*/

    if (first < grp->frstavl) first = grp->frstavl;
    if (last > grp->lastavl) last = grp->lastavl;
    if (first > last) return 1;

/*
**  Get the headers
*/
    if (first == last) {
    	sprintf(tmp, "XOVER %d", first);
    } else {
    	sprintf(tmp, "XOVER %d-%d", first, last);
    }
    server_send(tmp);
    server_get_reply(SRV__NOECHO, &reply_code, tmp, sizeof(tmp)-1, &len);
    if (reply_code != NNTP__DATAFOLLOWS) {
    	if (reply_code == NNTP__CMDUNRECOGNZD) news_cfg.xover = -1;
    	return 0;
    }
    last_valid = first-1;
    while (server_get_line(tmp, sizeof(tmp)-1, &len) != NEWS__EOLIST) {
    	tmp[len] = '\0';
    	cp = strchr(tmp, '\t');
    	if (cp == 0) continue;
    	*cp = '\0';
    	artnum = atoi(tmp);
    	if (artnum < first || artnum > last) continue;
/*
**  Look up this article in the cache, and add it if necessary
*/
    	ch = Cache_Lookup(artnum);
    	ch2 = Cache_Add(ch, artnum, 0);
/*
**  This is a valid article, so catch up on holes if needed
*/
    	if (artnum - last_valid > 1) {
    	    for (i = last_valid+1; i < artnum; i++) {
    	    	Mark_Article_Seen(grp, i);
    	    	grp->valid[i-grp->frstavl] = GRP_K_INVALID;
    	    }
    	}
    	last_valid = artnum;
    	grp->valid[artnum-grp->frstavl] = GRP_K_VALID;

/*
**  If we already have these headers, no need to parse them out again
*/
    	if (ch2->have_all_headers || ch2->have_xhdrs == XHDR_MASK) continue;

/*
**  Now let's get the headers from the response.
*/
    	anchor = cp + 1;
    	for (i = 0; anchor < (tmp + len) &&
    	    	    	i < sizeof(xover_hdrs)/sizeof(xover_hdrs[0]); i++) {
    	    cp = strchr(anchor, '\t');
    	    if (cp == 0) cp = tmp + len;
    	    if (xover_hdrs[i] < 0) {
    	    	anchor = cp + 1;
    	    	continue;
    	    }
    	    if (cp == anchor) {
    	    	ch2->xhdrs[xover_hdrs[i]] = 0;
    	    	ch2->have_xhdrs |= (1<<xover_hdrs[i]);
    	    	anchor = cp + 1;
    	    	continue;
    	    }

/*
**  If we already have this header, no need to add it to the list _again_.
*/
    	    xi = xover_hdrs[i];
    	    if (ch2->have_xhdrs & (1<<xi)) {
    	    	anchor = cp + 1;
    	    	continue;
    	    }

/*
**  Now parse out the header
*/
    	    *cp = '\0';
    	    j = strlen(anchor);
    	    h = mem_gethdr(j);
    	    strcpy(h->str, anchor);
    	    h->code = xhdr_codes[xi];
    	    h->len = j;
    	    queue_insert(h, ch2->hdrq.tail);
    	    if (xi >= 0) {
    	    	ch2->xhdrs[xi] = h;
    	    	ch2->have_xhdrs |= (1<<xi);
    	    }
    	    anchor = cp + 1;

    	}  /* loop through the XOVER response fields */

    } /* loop through the XOVER response lines */

/*
**  Final catch-up on holes, if needed
*/

    if (last_valid < last-1) {
    	for (i = last_valid + 1; i < last; i++) {
    	    Mark_Article_Seen(grp, i);
    	    grp->valid[i-grp->frstavl] = GRP_K_INVALID;
    	}
    }

    return 1;

} /* Preload_Cache */

/*
**++
**  ROUTINE:	Get_Article_Body
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Fetches an article body from the server (or out of our
**  single article cache).
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Get_Article_Body(int artnum, char *fspec, char *xrspec,
**  	    	    	    	int *xlines, int nosignal)
**
** artnum:  article number, read only, by value
** fspec:   file specification, read only, by reference (ASCIZ string)
** rspec:   file specification, write only, by reference (ASCIZ string)
** lines:   integer, write only, by reference
** nosignal: integer, read only, by value
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
unsigned int Get_Article_Body(int artnum, char *fspec, char *xrspec,
    	    	    	    	int *xlines, int nosignal) {

    char tmp[STRING_SIZE], tmp2[STRING_SIZE*2];
    int reply_code, tlen, lines;
    unsigned int status, unit;

/*
** We keep one article body around locally; check to see if that's what
** they want.  If so, copy it over and return.  Otherwise, clear things
** out for our new article.
*/
    if (artsave != NULL) {
    	if (curgroup == artsave->grpptr && artnum == artsave->artnum) {
    	    status = Copy_File(artsave->bodyfspec, fspec, xrspec, 0);
    	    if (OK(status)) return SS$_NORMAL;
    	    else wipe_article(&artsave);
    	}
    }

/*
** Create the file using the name they gave us, then fetch the body
** over from the server.
*/
    status = file_create(fspec, &unit, 0, xrspec);
    if (!OK(status)) {
    	if (!nosignal) lib$signal(NEWS__ARTRERR, 3, strlen(curgroup->grpnam),
    	    	    	curgroup->grpnam, artnum, status);
    	return status;
    }

    sprintf(tmp, "BODY %d", artnum);
    server_send(tmp);
    server_get_reply(SRV__NOECHO, &reply_code, tmp, sizeof(tmp), &tlen);
    if (reply_code != NNTP__BODYFOLLOWS) {
    	file_dclose(unit);
    	if (reply_code == NNTP__NOSUCHARTNUM || reply_code == NNTP__NOSUCHARTICLE) {
    	    if (curgroup->valid != 0) curgroup->valid[artnum-curgroup->frstavl] = GRP_K_INVALID;
    	    if (!nosignal) lib$signal(NEWS__ARTRERR, 3, strlen(curgroup->grpnam),
    	    	    	    	curgroup->grpnam, artnum, NEWS__NOSUCHARTICLE, 0);
    	    return NEWS__NOSUCHARTICLE;
    	} else {
    	    if (!nosignal) lib$signal(NEWS__ARTRERR, 3, strlen(curgroup->grpnam),
    	    	curgroup->grpnam, artnum, NEWS__UNEXPRSP, 3, reply_code, tlen, tmp);
    	    return NEWS__ARTRERR;
    	}
    }
    if (curgroup->valid != 0) curgroup->valid[artnum-curgroup->frstavl] = GRP_K_VALID;

    lines = 0;
    while (server_get_line(tmp, sizeof(tmp)-1, &tlen) != NEWS__EOLIST) {
    	lines++;
    	if (news_cfg.chrcnv) {
    	    *(tmp+tlen) = '\0';
    	    (*news_cfg.chrntol)(tmp, tmp2, sizeof(tmp2));
    	    status = file_write(unit, tmp2, strlen(tmp2));
    	} else {
    	    status = file_write(unit, tmp, tlen);
    	}
    	if (!OK(status)) {
    	    while (server_get_line(tmp, sizeof(tmp), &tlen) != NEWS__EOLIST);
    	    if (!nosignal) lib$signal(NEWS__ARTRERR, 3, strlen(curgroup->grpnam),
    	    	    curgroup->grpnam, artnum, status);
    	    file_dclose(unit);
    	    return status;
    	}
    }

    if (xlines) *xlines = lines;

    file_close(unit);
    return SS$_NORMAL;

} /* Get_Article_Body */

/*
**++
**  ROUTINE:	Ignore_Article
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Checks to see if the specified article should be ignored.
**
**  RETURNS:	boolean
**
**  PROTOTYPE:
**
**  	Ignore_Article(struct GRP *grp, int artnum)
**
** grp:	    GRP structure, read only, by reference
** artnum:  article number, read only, by value
**
**  IMPLICIT INPUTS:	news_prof.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    1: Article should be ignored.
**  	    0: Do not ignore this article.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int Ignore_Article(struct GRP *grp, int artnum) {

    struct QUE *hdrqptr, *kq[2];
    struct HDR *hdr;
    char tmp[STRING_SIZE+2];
    struct dsc$descriptor cand, pattern;
    int ignore, i, j;

#define IGNORE_COUNT 4
#define IGNORE_MASK (~((-1)<<IGNORE_COUNT))

    unsigned int ignore_flags;
    struct HDR *ignore_hdr[IGNORE_COUNT];
    struct {
    	int len;
    	char *str;
    } ignore_string[IGNORE_COUNT];
    static int ignore_code[IGNORE_COUNT] = {
    	NEWS_K_HDR_SUBJECT, NEWS_K_HDR_FROM, NEWS_K_HDR_MESSAGE_ID,
    	NEWS_K_HDR_NEWSGROUPS};
/*
** Check degenerate case - no local and no global kill lists.  Saves
** us fetching the headers unnecessarily.
*/
    if (grp->killq.head == &grp->killq &&
    	    news_prof.killq.head == &news_prof.killq) return 0;

/*
**  Now let's see which headers we will need.  Check local kill queue first.
*/
    ignore_flags = 0;
    for (hdr = grp->killq.head; hdr != (struct HDR *) &grp->killq &&
    	    	ignore_flags != IGNORE_MASK; hdr = hdr->flink) {
    	for (i = 0; i < IGNORE_COUNT; i++) {
    	    if (hdr->code == ignore_code[i]) {
    	    	ignore_flags |= (1<<i);
    	    	break;
    	    }
    	}
    }

/*
**  Now check global kill queue.
*/
    for (hdr = news_prof.killq.head; hdr != (struct HDR *) &news_prof.killq &&
    	    	ignore_flags != IGNORE_MASK; hdr = hdr->flink) {
    	for (i = 0; i < IGNORE_COUNT; i++) {
    	    if (hdr->code == ignore_code[i]) {
    	    	ignore_flags |= (1<<i);
    	    	break;
    	    }
    	}
    }

/*
**  If ignore_flags is still zero (it shouldn't be), forget it.
*/
    if (ignore_flags == 0) return 0;

/*
**  Set up things to begin the ignore check
*/
    ignore = 0;
    for (i = 0; i < IGNORE_COUNT; i++) {
    	ignore_hdr[i] = 0;
    	ignore_string[i].str = 0;
    }
/*
** Fetch the headers over.  If there's a problem, assume it's a dead
** article and tell the caller to ignore.
*/
    for (i = 0; i < IGNORE_COUNT; i++) {
    	if (ignore_flags & (1<<i)) {
    	    if (!OK(Get_One_Header(grp, artnum, ignore_code[i], &ignore_hdr[i]))) {
    	    	ignore = 1;
    	    	break;
    	    }
    	}
    	if (ignore_hdr[i] != 0) {
    	    ignore_string[i].len = ignore_hdr[i]->len;
    	    ignore_string[i].str = (char *) malloc(ignore_hdr[i]->len+1);
    	    if (ignore_string[i].str != 0) {
    	    	strcpy(ignore_string[i].str, ignore_hdr[i]->str);
    	    	upcase(ignore_string[i].str);
    	    }
    	}
    }

/*
** Now check the local and global kill lists to see if this article
** meets the criteria, unless it has already been marked for ignoring
** because the basic headers could not be retrieved.
*/
    if (!ignore) {
    	kq[0] = &grp->killq;
    	kq[1] = &news_prof.killq;
    	for (i = 0; !ignore && i < 2; i++) {
    	    for (hdr = kq[i]->head; !ignore && hdr != (struct HDR *) kq[i]; hdr = hdr->flink) {
    	    	if (!strchr(hdr->str, '*') && !strchr(hdr->str, '%')) {
    	    	    tmp[0] = '*';
    	    	    strcpy(&tmp[1], hdr->str);
    	    	    strcat(tmp, "*");
    	    	    INIT_SDESC(pattern, strlen(tmp), tmp);
    	    	} else {
    	    	    INIT_SDESC(pattern, strlen(hdr->str), hdr->str);
    	    	}
    	    	for (j = 0; !ignore && j < IGNORE_COUNT; j++) {
    	    	    if (hdr->code == ignore_code[j] && ignore_string[j].str != 0) {
    	    	    	INIT_SDESC(cand, ignore_string[j].len, ignore_string[j].str);
    	    	    	ignore = OK(str$match_wild(&cand, &pattern));
    	    	    }
    	    	}
    	    }
    	}
    }

/*
**  Free up the strings we allocated
*/
    for (i = 0; i < IGNORE_COUNT; i++) {
    	if (ignore_string[i].str != 0) free(ignore_string[i].str);
    }

/*
** If we are supposed to ignore this article, mark it seen as well.
*/
    if (ignore) Mark_Article_Seen(grp, artnum);

    return ignore;

} /* Ignore_Article */

/*
**++
**  ROUTINE:	Article_ExH
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Routine to be called by the NEWSRDR exit handler that
**  cleans up our one-article body cache.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Article_ExH()
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
unsigned int Article_ExH() {

    if (artsave != NULL) wipe_article(&artsave);

    return SS$_NORMAL;

} /* Article_ExH */

/*
**++
**  ROUTINE:	Edit_Article
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Puts an article into a file for viewing by an editor.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Edit_Article(struct ART **ctxp)
**
** ctxp: pointer to internally-defined structure, modify, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	    Normal successful completion.
**  	NEWS__EOARTICLE:    Normal completion; article has been fully displayed.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int Edit_Article(struct ART **ctxp) {

    struct ART *ctx = *ctxp;
    struct GRP *grp = (*ctxp)->grpptr;
    struct HDR *hdr;
    struct dsc$descriptor sdsc;
    char tmp[STRING_SIZE], rspec[FSPEC_SIZE];
    short tlen;
    int len, is_temp;
    unsigned int status, unit;
    static $DESCRIPTOR(alphabet, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    static $DESCRIPTOR(rotabet,  "NOPQRSTUVWXYZABCDEFGHIJKLMnopqrstuvwxyzabcdefghijklm");

    make_temp_fspec(tmp, sizeof(tmp));
    status = file_create(tmp, &unit, 0, rspec);
    if (!OK(status)) return status;
/*
** Header display
*/
    while (ctx->hlnptr != (struct HDR *) ctx->hdrqptr) {
    	int len;
    	Format_Header(ctx->hlnptr, tmp, sizeof(tmp), &len);
    	file_write(unit, tmp, len);
    	ctx->hlnptr = ctx->hlnptr->flink;
    }

    file_write(unit, "", 0);

    status = file_open(ctx->bodyfspec, &ctx->bodyunit, 0, 0, 0);
    if (!OK(status)) {
    	lib$signal(NEWS__BODYERR, 2, strlen(ctx->bodyfspec),
    	    	    	    ctx->bodyfspec, status);
    	file_dclose(unit);
    	return status;
    }

/*
** Display of article body
*/
    while (OK(file_read(ctx->bodyunit, tmp, sizeof(tmp)-1, &len))) {

    	if (rotate_text) {
    	    struct dsc$descriptor tdsc;
    	    char tmp2[STRING_SIZE];
    	    INIT_SDESC(sdsc, len, tmp);
    	    INIT_SDESC(tdsc, len, tmp2);
    	    str$translate(&tdsc, &sdsc, &alphabet, &rotabet);
    	    file_write(unit, tmp2, len);
    	} else file_write(unit, tmp, len);
    }

    file_close(unit);
    file_close(ctx->bodyunit);
    ctx->bodyunit = 0;
    artsave = ctx;

    status = Compose_Message(rspec, tmp, 1, &is_temp);
    if (OK(status) && is_temp) file_delete(tmp);
    file_delete(rspec);

    return status;

} /* Edit_Article */

/*
**++
**  ROUTINE:	Check_For_Holes
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Uses the XHDR command to check a newsgroup for holes.
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
void Check_For_Holes(struct GRP *grp, int first, int last) {

    char tmp[STRING_SIZE];
    unsigned int status;
    int i, j, last_valid, len, reply_code, old_min;
    char *cp;

    if (news_cfg.xover >= 0) {
    	if (Preload_Cache(grp, first, last)) return;
    }

    if (news_cfg.xhdr < 0) return;

    if (first < grp->frstavl) first = grp->frstavl;
    if (last > grp->lastavl) last = grp->lastavl;

    if (first > last) return;

    sprintf(tmp, "XHDR Subject %d-%d", first, last);
    server_send(tmp);
    server_get_reply(SRV__NOECHO, &reply_code, tmp, sizeof(tmp)-1, &len);
    if (reply_code != NNTP__HEADFOLLOWS) {
    	if (reply_code == NNTP__CMDUNRECOGNZD) news_cfg.xhdr = -1;
    	return;
    }
    news_cfg.xhdr = 1;

    last_valid = first-1;
    while (server_get_line(tmp, sizeof(tmp)-1, &len) != NEWS__EOLIST) {
    	int i = 0;
    	tmp[len] = '\0';
    	for (cp = tmp; *cp != '\0' && !isspace(*cp); cp++) {
    	    if (!isdigit(*cp)) {
    	    	i = 0;
    	    	break;
    	    }
    	    i = i * 10 + (*cp - '0');
    	}
    	if (i == 0) continue;
    	if (i - last_valid > 1) {
    	    for (j = last_valid+1; j < i; j++) {
    	    	Mark_Article_Seen(grp, j);
    	    	grp->valid[j-grp->frstavl] = GRP_K_INVALID;
    	    }
    	}
    	last_valid = i;
    	grp->valid[i-grp->frstavl] = GRP_K_VALID;
    }

    if (last_valid < last-1) {
    	for (j = last_valid + 1; j < last; j++) {
    	    Mark_Article_Seen(grp, j);
    	    grp->valid[j-grp->frstavl] = GRP_K_INVALID;
    	}
    }

    return;

} /* Check_For_Holes */

/*
**++
**  ROUTINE:	Is_Valid_Article
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Checks to see if an article is on the invalid-article list.
**
**  RETURNS:	int (1 or 0)
**
**  PROTOTYPE:
**
**  	Is_Valid_Article(struct GRP *grp, int artnum)
**
** artnum:  article number, read only, by value
** grp:	    GRP structure, read only, by reference
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	   >0:	article is valid
**  	   <0:	article may be valid, we don't know
**  	    0:  article definitely not valid
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int Is_Valid_Article(struct GRP *grp, int artnum) {

    struct RNG *r;
    int base, a;

    if (grp == NULL) grp = curgroup;

    if (artnum > grp->lastavl) return 0;

    if (grp->valid == 0) {
    	a = grp->lastavl-grp->frstavl+1;
    	grp->valid = malloc(a);
    	if (grp->valid == 0) return -1;
    	memset(grp->valid, 0, a);
    }

    base = artnum - grp->frstavl;
    if (grp->valid[base] == 0) {
    	for (a = 1; artnum+a <= grp->lastavl &&
    	    	grp->valid[base+a] == 0 && a < 11; a++);
    	Check_For_Holes(grp, artnum, artnum+a-1);
    }
    if (grp->valid[base] == 0) return -1;

    return grp->valid[base] == GRP_K_VALID ? 1 : 0;

} /* Is_Valid_Article */

/*
**++
**  ROUTINE:	Article_Seen
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Checks to see if an article is on the invalid-article list.
**
**  RETURNS:	int (1 or 0)
**
**  PROTOTYPE:
**
**  	Article_Seen(struct GRP *grp, int artnum)
**
** artnum:  article number, read only, by value
** grp:	    GRP structure, read only, by reference
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    1:	article seen
**  	    0:  article not seen
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int Article_Seen(struct GRP *grp, int artnum) {

    struct RNG *r;
    int a;

    if (grp == NULL) grp = curgroup;

    if (artnum > grp->lastavl) return 0;

/*
** Check to see if it's on the invalid article list
*/
    for (r = grp->seenq.tail; r != (struct RNG *) &grp->seenq; r = r->blink) {
    	if (artnum >= r->first) break;
    }
    if (r != (struct RNG *) &grp->seenq) {
    	if (artnum <= r->last) return 1;
    }

    return 0;

} /* Article_Seen */

/*
**++
**  ROUTINE:	Cache_Init
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Initializes the header cache.
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
static void Cache_Init(struct GRP *grp) {

    mem_delcache();
    mem_initcache(sizeof(struct CHDR));
    hdrcache.head = hdrcache.tail = &hdrcache;
    hcgroup = grp;
    hccount = 0;
}

/*
**++
**  ROUTINE:	Cache_Lookup
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Looks up an article in the header cache.
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
static struct CHDR *Cache_Lookup(int artnum) {

    struct CHDR *ch;

    if (hdrcache.head != &hdrcache &&
    	    artnum > ((struct CHDR *) hdrcache.tail)->artnum) {
    	hdrcache_remove_from_tail = 0;
    	return 0;  /* short cut for common case */
    } else {
    	for (ch = hdrcache.head; ch != (struct CHDR *) &hdrcache; ch = ch->flink) {
    	    if (artnum <= ch->artnum) {
    	    	if (ch == hdrcache.head) hdrcache_remove_from_tail = 1;
    	    	return ch;
    	    }
    	}
    }

    hdrcache_remove_from_tail = 1;

    return 0;

} /* Cache_Lookup */

/*
**++
**  ROUTINE:	Cache_Add
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
static struct CHDR *Cache_Add(struct CHDR *insert_point, int artnum, int clear) {

    struct CHDR *ch;
    struct HDR *hdr;

    if (insert_point != 0 && insert_point->artnum == artnum) {
    	ch = insert_point;
    	if (!clear) return ch;
    	while (queue_remove(ch->hdrq.head, &hdr)) mem_freehdr(hdr);
    } else {
    	if (hccount >= news_cfg.cachesize) {
    	    if (hdrcache_remove_from_tail)
    	    	queue_remove(hdrcache.tail, &ch);
    	    else
    	    	queue_remove(hdrcache.head, &ch);
    	} else {
    	    ch = mem_getcache();
    	    hccount += 1;
    	}
    	if (insert_point == 0)
    	    queue_insert(ch, hdrcache.tail);
    	else
    	    queue_insert(ch, insert_point->blink);
    }
    memset(ch->xhdrs, 0, sizeof(ch->xhdrs));
    ch->hdrq.head = ch->hdrq.tail = &ch->hdrq;
    ch->artnum = artnum;
    ch->have_xhdrs = 0;
    ch->have_all_headers = 0;

    return ch;

} /* Cache_Add */
