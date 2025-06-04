/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	PRINT commands
**
**  MODULE DESCRIPTION:
**
**  	This module contains the routines that implement the PRINT
**  commands for NEWSRDR.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, 1994 MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  10-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	10-SEP-1992 V1.0    Madison 	Initial coding.
**  	15-FEB-1993 V1.0-1  Madison 	Fix for pre-VMS V5.2 compatibility.
**  	23-MAR-1993 V1.0-2  Madison 	Fix file_read reference.
**  	12-APR-1993 V1.1    Madison 	Get_Article_* routines changed.
**  	14-MAY-1993 V1.2    Madison 	Wasn't taking queue name from profile!
**  	15-JAN-1994 V1.2-1  Madison 	Get_Article_Body has changed.
**--
*/
#include "newsrdr.h"
#include "globals.h"
#ifdef __GNUC__
#include <vms/rms.h>
#include <vms/sjcdef.h>
#else
#include <rms.h>
#include <sjcdef.h>
#endif

    struct PRT {
    	TIME	after;
    	struct FAB fab;
    	struct RAB rab;
    	struct NAM nam;
    	int 	burst, copies, feed, flag, hold, notify, space;
    	int 	trailer, active;
    	char	form[33], name[40], queue[33], param[8][256];
    };

/*
** Forward declarations
*/
    unsigned int cmd_print(void);
    unsigned int cmd_print_now(void);
    unsigned int cmd_print_cancel(void);
    unsigned int print_clup(void);
    static int characteristics_match(struct PRT *, struct PRT *);

    static struct PRT *old_prt = NULL;
    static $DESCRIPTOR(alphabet, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    static $DESCRIPTOR(rotabet,  "NOPQRSTUVWXYZABCDEFGHIJKLMnopqrstuvwxyzabcdefghijklm");

    EXTERN struct GRP *curgroup;
    EXTERN int        cur_artnum;

    extern unsigned int Parse_Range(char *, int *, int *, int, struct GRP *);
    extern unsigned int Get_Article_Hdrs(struct GRP *, int, void *, int, int *);
    extern unsigned int Get_Article_Body(int, char *, char *, int *, int);

/*
**++
**  ROUTINE:	cmd_print
**
**  FUNCTIONAL DESCRIPTION:
**
**  	PRINT command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_print()
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
unsigned int cmd_print() {

    char tmp[STRING_SIZE], bodyspec[FSPEC_SIZE];
    struct QUE rangeq, *hdrq;
    struct RNG *rng;
    struct HDR *hdr;
    struct PRT *prt;
    struct dsc$descriptor tdsc;
    int rstart, rend, did_one, do_headers, rotate;
    int append, len;
    unsigned int unit, status;

    if (curgroup == NULL) {
    	lib$signal(NEWS__NOCURGROUP, 0);
    	return SS$_NORMAL;
    }

/*
** First, get all the parameters and stuff we need to set up the
** PRT context block.
*/
    do_headers = cli_present("HEADERS") != CLI$_NEGATED;
    rotate = cli_present("ROTATE") == CLI$_PRESENT;
    rangeq.head = rangeq.tail = &rangeq;
    if (cli_present("ARTRNG") == CLI$_PRESENT) {
    	while (OK(cli_get_value("ARTRNG", tmp, sizeof(tmp)))) {
    	    if (!OK(Parse_Range(tmp, &rstart, &rend, cur_artnum, curgroup))) {
    	    	lib$signal(NEWS__INVARTRNG, 2, strlen(tmp), tmp);
    	    	while (queue_remove(rangeq.head, &rng)) mem_freerng(rng);
    	    	return SS$_NORMAL;
    	    }
    	    if (rstart > rend) {
    	    	int i;
    	    	i = rstart; rstart = rend; rend = i;
    	    }
    	    if (rend > curgroup->lastavl) rend = curgroup->lastavl;
    	    if (rend < curgroup->frstavl) rend = curgroup->frstavl;
    	    if (rstart > curgroup->lastavl) rstart = curgroup->lastavl;
    	    if (rstart < curgroup->frstavl) rstart = curgroup->frstavl;
    	    rng = mem_getrng();
    	    rng->first = rstart;
    	    rng->last = rend;
    	    queue_insert(rng, rangeq.tail);
    	}
    } else {
    	if (cur_artnum == 0) {
    	    lib$signal(NEWS__NOCURART, 0);
    	    return SS$_NORMAL;
    	}
    	rng = mem_getrng();
    	rng->first = rng->last = cur_artnum;
    	queue_insert(rng, rangeq.tail);
    }

    prt = malloc(sizeof(struct PRT));
    memset(prt, 0, sizeof(struct PRT));

    if (cli_present("AFTER") == CLI$_PRESENT) {
    	cli_get_value("AFTER", tmp, sizeof(tmp));
    	INIT_SDESC(tdsc, strlen(tmp), tmp);
    	status = lib$convert_date_string(&tdsc, &(prt->after));
    	if (!OK(status)) {
    	    lib$signal(NEWS__CMDERR, 0, status);
    	    free(prt);
    	    while (queue_remove(rangeq.head, &rng)) mem_freerng(rng);
    	    return SS$_NORMAL;
    	}
    }

    status = cli_present("BURST");
    if (status == CLI$_PRESENT) {
    	static char *all = "ALL";
    	if (OK(cli_get_value("BURST", tmp, sizeof(tmp)))) {
    	    prt->burst = (strstr(all, tmp) == all) ? SJC$_FILE_BURST :
    	    	    	    SJC$_FILE_BURST_ONE;
    	} else {
    	    prt->burst = SJC$_FILE_BURST;
    	}
    } else if (status == CLI$_NEGATED) {
    	prt->burst = SJC$_NO_FILE_BURST;
    }

    if (cli_present("COPIES") == CLI$_PRESENT) {
    	cli_get_value("COPIES", tmp, sizeof(tmp));
    	lib$cvt_dtb(strlen(tmp), tmp, &(prt->copies));
    	if (prt->copies > 255) prt->copies = 255;
    	if (prt->copies < 1) prt->copies = 1;
    } else {
    	prt->copies = 1;
    }

    status = cli_present("FEED");
    if (status == CLI$_PRESENT) {
    	prt->feed = SJC$_PAGINATE;
    } else if (status == CLI$_NEGATED) {
    	prt->feed = SJC$_NO_PAGINATE;
    }

    status = cli_present("FLAG");
    if (status == CLI$_PRESENT) {
    	static char *all = "ALL";
    	if (OK(cli_get_value("FLAG", tmp, sizeof(tmp)))) {
    	    prt->flag = (strstr(all, tmp) == all) ? SJC$_FILE_FLAG :
    	    	    	    SJC$_FILE_FLAG_ONE;
    	} else {
    	    prt->flag = SJC$_FILE_FLAG;
    	}
    } else if (status == CLI$_NEGATED) {
    	prt->flag = SJC$_NO_FILE_FLAG;
    }

    if (cli_present("FORM") == CLI$_PRESENT) {
    	cli_get_value("FORM", prt->form, sizeof(prt->form));
    } else {
    	strcpy(prt->form, news_prof.print_form);
    }

    status = cli_present("HOLD");
    if (status == CLI$_PRESENT) {
    	prt->hold = SJC$_HOLD;
    } else if (status == CLI$_NEGATED) {
    	prt->hold = SJC$_NO_HOLD;
    }

    if (cli_present("NAME") == CLI$_PRESENT) {
    	cli_get_value("NAME", prt->name, sizeof(prt->name));
    }

    status = cli_present("NOTIFY");
    if (status == CLI$_PRESENT) {
    	prt->notify = SJC$_NOTIFY;
    } else if (status == CLI$_NEGATED) {
    	prt->notify = SJC$_NO_NOTIFY;
    }

    if (cli_present("PARAMETERS") == CLI$_PRESENT) {
    	int i;
    	for (i = 0; i < 8; i++) {
    	    if (!OK(cli_get_value("PARAMETERS", prt->param[i],
    	    	    	    	    sizeof(prt->param[i])))) break;
    	}
    }

    if (cli_present("QUEUE") == CLI$_PRESENT) {
    	cli_get_value("QUEUE", prt->queue, sizeof(prt->queue));
    } else {
    	strcpy(prt->queue, news_prof.print_queue);
    }

    status = cli_present("SPACE");
    if (status == CLI$_PRESENT) {
    	prt->space = SJC$_DOUBLE_SPACE;
    } else if (status == CLI$_NEGATED) {
    	prt->space = SJC$_NO_DOUBLE_SPACE;
    }

    status = cli_present("TRAILER");
    if (status == CLI$_PRESENT) {
    	static char *all = "ALL";
    	if (OK(cli_get_value("TRAILER", tmp, sizeof(tmp)))) {
    	    prt->flag = (strstr(all, tmp) == all) ? SJC$_FILE_TRAILER :
    	    	    	    SJC$_FILE_TRAILER_ONE;
    	} else {
    	    prt->flag = SJC$_FILE_TRAILER;
    	}
    } else if (status == CLI$_NEGATED) {
    	prt->flag = SJC$_NO_FILE_TRAILER;
    }

/*
** Now that we've built the PRT block for this request, compare it
** against the old one (if there is one).  If it's a match, then fine.
** Otherwise, clean up the old and start with the new one.
*/
    did_one = 0;
    if (characteristics_match(prt, old_prt)) {
    	free(prt);
    	did_one = 1;
    } else {
    	print_clup();
    	make_temp_fspec(tmp, sizeof(tmp));
    	prt->fab = cc$rms_fab;
    	prt->fab.fab$l_fna = tmp;
    	prt->fab.fab$b_fns = strlen(tmp);
    	prt->fab.fab$b_fac = FAB$M_PUT;
    	prt->fab.fab$b_rat = FAB$M_CR;
    	prt->fab.fab$l_nam = &(prt->nam);
    	prt->rab = cc$rms_rab;
    	prt->rab.rab$l_fab = &(prt->fab);
    	prt->nam = cc$rms_nam;
    	status = sys$create(&(prt->fab),0,0);
    	if (!OK(status)) {
    	    lib$signal(NEWS__PRINTERR, 0, status, prt->fab.fab$l_stv);
    	    free(prt);
    	    while(queue_remove(rangeq.head, &rng)) mem_freerng(rng);
    	    return SS$_NORMAL;
    	}
    	sys$connect(&(prt->rab),0,0);
    	old_prt = prt;
    }

    prt = old_prt;

/*
** Now we add the article(s) to the print file.
*/
    while (queue_remove(rangeq.head, &rng)) {
    	int i;
    	for (i = rng->first; i <= rng->last; i++) {
    	    if (do_headers) {
    	    	status = Get_Article_Hdrs(curgroup, i, &hdrq, 1, 0);
    	    	if (!OK(status)) lib$signal(NEWS__PRINTERR, 1, i, status, 0);
    	    }
    	    if ((do_headers && OK(status)) || !do_headers) {
    	    	char bodyspec[FSPEC_SIZE];
    	    	make_temp_fspec(tmp, sizeof(tmp));
    	    	status = Get_Article_Body(i, tmp, bodyspec, 0, 0);
    	    	if (!OK(status)) {
    	    	    lib$signal(NEWS__PRINTERR, 1, i, status, 0);
    	    	} else {
    	    	    if (did_one) {
    	    	    	prt->rab.rab$l_rbf = "\014";
    	    	    	prt->rab.rab$w_rsz = 1;
    	    	    	sys$put(&(prt->rab),0,0);
    	    	    }
    	    	    did_one = 1;
    	    	    prt->rab.rab$l_rbf = tmp;
    	    	    if (do_headers) {
    	    	    	for (hdr = hdrq->head; hdr != (struct HDR *) hdrq;
    	    	    	    	    	hdr = hdr->flink) {
    	    	    	    int hlen;
    	    	    	    Format_Header(hdr, tmp, sizeof(tmp), &hlen);
    	    	    	    prt->rab.rab$w_rsz = hlen;
    	    	    	    sys$put(&(prt->rab),0,0);
    	    	    	}
    	    	    	prt->rab.rab$w_rsz = 0;
    	    	    	sys$put(&(prt->rab),0,0);
    	    	    }
    	    	    file_open(bodyspec, &unit, 0, 0, 0);
    	    	    while (OK(file_read(unit, tmp, sizeof(tmp)-1, &len))) {
    	    	    	prt->rab.rab$w_rsz = len;
    	    	    	if (rotate) {
    	    	    	    struct dsc$descriptor sdsc;
    	    	    	    char tmp2[STRING_SIZE];
    	    	    	    INIT_SDESC(sdsc, len, tmp2);
    	    	    	    INIT_SDESC(tdsc, len, tmp);
    	    	    	    str$translate(&sdsc, &tdsc, &alphabet, &rotabet);
    	    	    	    prt->rab.rab$l_rbf = tmp2;
    	    	    	    sys$put(&(prt->rab),0,0);
    	    	    	    prt->rab.rab$l_rbf = tmp;
    	    	    	} else {
    	    	    	    sys$put(&(prt->rab),0,0);
    	    	    	}
    	    	    }
    	    	    file_close(unit);
    	    	    file_delete(bodyspec);
    	    	}
    	    }
    	}
    	mem_freerng(rng);
    }

    return SS$_NORMAL;

} /* cmd_print */

/*
**++
**  ROUTINE:	cmd_print_now
**
**  FUNCTIONAL DESCRIPTION:
**
**  	PRINT/NOW command.  Sends the pending print job to the queue.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_print_now()
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
unsigned int cmd_print_now() {

    unsigned int status;

    if (old_prt == NULL) {
    	lib$signal(NEWS__NOPRINT, 0);
    } else {
    	status = cli_present("NOTIFY");
    	if (status == CLI$_PRESENT) old_prt->notify = SJC$_NOTIFY;
    	else if (status == CLI$_NEGATED) old_prt->notify = SJC$_NO_NOTIFY;
    	print_clup();
    }
    return SS$_NORMAL;

} /* cmd_print_now */

/*
**++
**  ROUTINE:	cmd_print_cancel
**
**  FUNCTIONAL DESCRIPTION:
**
**  	PRINT/CANCEL command.  Destroys the current print job.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cmd_print_cancel()
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
unsigned int cmd_print_cancel() {

    if (old_prt == NULL) {
    	lib$signal(NEWS__NOPRINT, 0);
    } else {
    	sys$disconnect(&(old_prt->rab),0,0);
    	old_prt->fab.fab$l_fop |= FAB$M_DLT;
    	sys$close(&(old_prt->fab),0,0);
    	free(old_prt);
    	old_prt = NULL;
    }
    return SS$_NORMAL;
} /* cmd_print_cancel */

/*
**++
**  ROUTINE:	print_clup
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Submits the current pending print job to the queue.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	print_clup()
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
unsigned int print_clup() {

    static int sjcpar[8] = {SJC$_PARAMETER_1, SJC$_PARAMETER_2,
                    SJC$_PARAMETER_3, SJC$_PARAMETER_4,
                    SJC$_PARAMETER_5, SJC$_PARAMETER_6,
                    SJC$_PARAMETER_7, SJC$_PARAMETER_8};
    static TIME zerotime = {0,0};

#define ADD_ITEM(code) {if (code) {ITMLST_INIT(jbclst[j],code,0,0,0); j++;}}

    struct PRT *prt = old_prt;
    ITMLST jbclst[25];
    unsigned short iosb[4], statlen;
    char statmsg[256];
    int i, j;
    unsigned int status;

    if (prt == NULL) return SS$_NORMAL;

    sys$disconnect(&(prt->rab),0,0);
    sys$close(&(prt->fab),0,0);
    j = 0;

    if (memcmp(&prt->after, &zerotime, sizeof(TIME)) != 0) {
    	ITMLST_INIT(jbclst[j], SJC$_AFTER_TIME, sizeof(prt->after),
    	    	    	    	    &(prt->after), 0);
    	j++;
    }
    if (prt->copies) {
    	ITMLST_INIT(jbclst[j], SJC$_FILE_COPIES, sizeof(prt->copies),
    	    	    	    	    &(prt->copies), 0);
    	j++;
    }
    ADD_ITEM(prt->burst)
    ADD_ITEM(prt->feed)
    ADD_ITEM(prt->flag)
    ADD_ITEM(prt->hold)
    ADD_ITEM(prt->notify)
    ADD_ITEM(prt->space)
    ADD_ITEM(prt->trailer)

    if (*(prt->form)) {
    	ITMLST_INIT(jbclst[j], SJC$_FORM_NAME, strlen(prt->form), prt->form, 0);
    	j++;
    }
    for (i = 0; i < 8; i++) {
    	if (*(prt->param[i])) {
    	    ITMLST_INIT(jbclst[j], sjcpar[i], strlen(prt->param[i]),
    	    	    	    	prt->param[i], 0);
    	    j++;
    	}
    }

    if (*(prt->queue)) {
    	ITMLST_INIT(jbclst[j], SJC$_QUEUE, strlen(prt->queue), prt->queue, 0);
    } else if (*news_prof.print_queue) {
    	ITMLST_INIT(jbclst[j], SJC$_QUEUE, strlen(news_prof.print_queue),\
    	    	    	    	    	    	    news_prof.print_queue, 0);
    } else {
    	ITMLST_INIT(jbclst[j], SJC$_QUEUE, 9, "SYS$PRINT", 0);
    }
    j++;

    if (*(prt->name)) {
    	ITMLST_INIT(jbclst[j], SJC$_JOB_NAME, strlen(prt->name), prt->name, 0);
    } else {
    	ITMLST_INIT(jbclst[j], SJC$_JOB_NAME, 4, "NEWS", 0);
    }
    j++;

    ITMLST_INIT(jbclst[j], SJC$_FILE_IDENTIFICATION, 28,
    	    	    	&(prt->nam.nam$t_dvi), 0);
    j++;
    ITMLST_INIT(jbclst[j], SJC$_DELETE_FILE, 0, 0, 0);
    j++;
    statlen = 0;
    ITMLST_INIT(jbclst[j], SJC$_JOB_STATUS_OUTPUT, sizeof(statmsg)-1,
    	    	    	statmsg, &statlen);
    j++;
    ITMLST_INIT(jbclst[j], 0, 0, 0, 0);

    status = sys$sndjbcw(0, SJC$_ENTER_FILE, 0, jbclst, iosb, 0, 0);
    if (OK(status)) status = iosb[0];
    if (!OK(status)) lib$signal(NEWS__PRINTERR, 0, status);
    if (statlen > 0) put_output_counted(statmsg, statlen);
    free(prt);

    old_prt = NULL;

    return SS$_NORMAL;

} /* print_clup */

/*
**++
**  ROUTINE:	characteristics_match
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Checks to see if one PRT block's characterstics match another's.
**  For compatibility with VMS Mail, all we check are the queue name
**  and the PARAMETERs values.
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
static int characteristics_match(struct PRT *p1, struct PRT *p2) {

    int i;

    if (p1 == NULL || p2 == NULL) return 0;

    if (strcmp(p1->queue, p2->queue) != 0) return 0;
    for (i = 0; i < 8; i++) {
    	if (strcmp(p1->param[i], p2->param[i]) != 0) return 0;
    }

    return 1;

} /* characteristics_match */
