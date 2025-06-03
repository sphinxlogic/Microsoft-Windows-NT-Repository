/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Read and write .newsrc files.
**
**  MODULE DESCRIPTION:
**
**  	This module contains the routines that read and write .newsrc
**  files.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, 1994  MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  11-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	11-SEP-1992 V1.0    Madison 	Initial coding.
**  	15-FEB-1993 V1.0-1  Madison 	Make compatible with pre-VMS V5.2.
**  	24-SEP-1993 V1.1    Madison 	mem_getgrp now inits queues.
**  	17-MAY-1994 V1.2    Madison 	Turn on EXQUOTA if we have it.
**  	22-MAY-1994 V1.2-1  Madison 	Make write more like profile write.
**  	28-JUN-1994 V1.2-2  Madison 	Condense "n-n" to just "n".
**--
*/
#include "newsrdr.h"
#include "globals.h"
#ifdef __GNUC__
#include <vms/rms.h>
#include <vms/prvdef.h>
#else
#include <rms.h>
#include <prvdef.h>
#endif

    void Read_NewsRC();
    void Write_NewsRC();
    static unsigned int write_newsrc_info(struct GRP *, struct RAB *);

    extern struct GRP *Find_Group(char *);
    extern void Insert_Group(struct GRP *);
    extern unsigned int Parse_Range(char *, int *, int *, int, struct GRP *);

/*
**++
**  ROUTINE:	Read_NewsRC
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Reads in a .newsrc file.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Read_NewsRC()
**
**  IMPLICIT INPUTS:	Many.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void Read_NewsRC() {

    char tmp[STRING_SIZE], name[STRING_SIZE];
    char *anchor, *cp;
    struct RNG *rng;
    struct GRP *g;
    int tlen, f, l;
    unsigned int status, unit;

    status = file_open(news_prof.newsrc, &unit, 0, 0, 0);
    if (!OK(status)) {
    	lib$signal(NEWS__RCRDERR, 2, strlen(news_prof.newsrc),
    	    	    	news_prof.newsrc, status);
    	return;
    }

    while (OK((status = file_read(unit, tmp, sizeof(tmp)-1, &tlen)))) {
    	tmp[tlen] = '\0';
    	cp = strchr(tmp, ':');
    	if (!cp) cp = strchr(tmp, '!');
    	if (!cp) continue;
    	strncpy(name, tmp, cp-tmp);
    	*(name+(cp-tmp)) = '\0';
    	g = Find_Group(name);
    	if (g == NULL) {
    	    g = mem_getgrp();
    	    strcpy(g->grpnam, name);
    	    g->inprofile = 1;
    	    Insert_Group(g);
    	}
    	g->subscribed = (*cp == ':');
    	if (news_prof.glast) {
    	    news_prof.glast->next = g;
    	} else {
    	    news_prof.glist = g;
    	}
    	news_prof.glast = g;
    	while (queue_remove(g->seenq.head, &rng)) mem_freerng(rng);
    	for (anchor = ++cp; anchor; anchor = cp) {
    	    cp = strchr(anchor, ',');
    	    if (cp) *cp++ = '\0';
    	    if (OK(Parse_Range(anchor, &f, &l, 0, 0))) {
    	    	rng = mem_getrng();
    	    	rng->first = f;
    	    	rng->last = l;
    	    	queue_insert(rng, g->seenq.tail);
    	    }
    	}
    }
    if (status != RMS$_EOF) {
    	lib$signal(NEWS__RCRDERR, 2, strlen(news_prof.newsrc),
    	    	    	news_prof.newsrc, status);
    }

    file_close(unit);

    return;

} /* Read_NewsRC */

/*
**++
**  ROUTINE:	Write_NewsRC
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Writes a .newsrc file.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Write_NewsRC()
**
**  IMPLICIT INPUTS:	Many.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void Write_NewsRC() {

    struct FAB fab;
    struct NAM nam;
    struct RAB rab;
    unsigned int status, pp[2], pstat;
    char espec[255], rspec[255];
    static unsigned int exquota[2] = {PRV$M_EXQUOTA, 0};

    pstat = sys$setprv(1, exquota, 0, pp);

    fab = cc$rms_fab;
    fab.fab$b_fac = FAB$M_PUT;
    fab.fab$l_fna = news_prof.newsrc;
    fab.fab$b_fns = strlen(news_prof.newsrc);
    fab.fab$l_dna = "";
    fab.fab$b_dns = 0;
    fab.fab$b_rat = FAB$M_CR;

    nam = cc$rms_nam;
    nam.nam$l_esa = espec;
    nam.nam$b_ess = sizeof(espec);
    nam.nam$l_rsa = rspec;
    nam.nam$b_rss = sizeof(rspec);
    fab.fab$l_nam = (void *) &nam;

    status = sys$create(&fab,0,0);
    if (!OK(status)) {
    	lib$signal(NEWS__RCWRTERR, 2, strlen(news_prof.newsrc),
    	    	    	news_prof.newsrc, status, fab.fab$l_stv);
    	if (OK(pstat) && !(pp[0] & PRV$M_EXQUOTA)) sys$setprv(0, exquota, 0, 0);
    	return;
    }

    rab = cc$rms_rab;
    rab.rab$l_fab = &fab;
    sys$connect(&rab, 0, 0);

    if (news_prof.glist) {
    	struct GRP *g;
    	for (g = news_prof.glist; g != NULL; g = g->next) {
    	    status = write_newsrc_info(g, &rab);
    	    if (!OK(status)) break;
    	}
    } else {
    	lib$traverse_tree(&news_prof.gtree, write_newsrc_info, &rab);
    	status = rab.rab$l_sts;
    }

    if (!OK(status)) lib$signal(NEWS__RCWRTERR, 2,
    	    nam.nam$b_rsl, nam.nam$l_rsa, status, rab.rab$l_stv);

    sys$disconnect(&rab, 0, 0);
    if (!OK(status)) fab.fab$l_fop |= FAB$M_DLT;
    sys$close(&fab, 0, 0);

    if (news_prof.newsrc_purge && OK(status)) {
    	struct dsc$descriptor dsc, ddsc, fname;
    	static $DESCRIPTOR(dname, ";*");
    	static $DESCRIPTOR(fname_ren, ";1");
    	unsigned int ctx;

    	ddsc.dsc$b_dtype = DSC$K_DTYPE_T;
    	ddsc.dsc$b_class = DSC$K_CLASS_D;
    	ddsc.dsc$w_length = 0;
    	ddsc.dsc$a_pointer = 0;
    	INIT_SDESC(dsc, nam.nam$b_rsl, nam.nam$l_rsa);
    	INIT_SDESC(fname, fab.fab$b_fns, fab.fab$l_fna);
    	ctx = 0;
    	while (OK(lib$find_file(&fname, &ddsc, &ctx, &dname))) {
    	    if (str$compare_eql(&ddsc, &dsc) != 0) lib$delete_file(&ddsc);
    	}
    	lib$rename_file(&dsc, &fname_ren);
    	str$free1_dx(&ddsc);
    }

    if (OK(pstat) && !(pp[0] & PRV$M_EXQUOTA)) sys$setprv(0, exquota, 0, 0);

} /* Write_NewsRC */

/*
**++
**  ROUTINE:	write_newsrc_info
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Tree traversal action routine that writes group information
**  to the newsrc file.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	write_newsrc_info(struct GRP *g, unsigned int unit)
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
static unsigned int write_newsrc_info(struct GRP *g, struct RAB *rab) {

    char tmp2[40];
    char *cp;
    struct RNG *rng;
    unsigned int status;
    int i;
    static char *tmp = 0;
    static int tmpsize;

    if (!g->avail) return SS$_NORMAL;
    if (news_prof.newsrc_subonly && !g->subscribed) return SS$_NORMAL;

    if (tmp == 0) {
    	tmp = malloc(STRING_SIZE);
    	tmpsize = STRING_SIZE;
    }

    sprintf(tmp, "%s%c", g->grpnam, (g->subscribed ? ':' : '!'));
    cp = tmp+strlen(tmp);

    for (rng = g->seenq.head; rng != (struct RNG *) &(g->seenq); rng = rng->flink) {
    	if (cp-tmp > tmpsize-40) {
    	    tmpsize += STRING_SIZE;
    	    tmp = realloc(tmp, tmpsize);
    	}
    	if (rng->last >= g->frstavl) {
    	    i = (rng->first < g->frstavl ? 1 : rng->first);
    	    if (i == rng->last) sprintf(tmp2, "%d,", i);
    	    else sprintf(tmp2, "%d-%d,", i, rng->last);
    	    strcat(tmp, tmp2);
    	    cp += strlen(tmp2);
    	}
    }

    if (*(cp-1) == ',') *(--cp) = '\0';

    rab->rab$l_rbf = tmp;
    rab->rab$w_rsz = cp - tmp;
    status = sys$put(rab, 0, 0);

    return status;

} /* write_newsrc_info */
