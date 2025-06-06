/*
**++
**  FACILITY:	MMK
**
**  ABSTRACT:	Subprocess manager
**
**  MODULE DESCRIPTION:
**
**  	This module contains routines for managing subprocesses
**  used by MMK.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT � 1992-1995, MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  21-AUG-1992
**
**  MODIFICATION HISTORY:
**
**  	21-AUG-1992 V1.0    Madison 	Initial coding.
**  	01-SEP-1992 V1.1    Madison 	Comments.
**  	20-OCT-1993 V1.2    Madison 	Add sp_show_subprocess.
**  	29-JUN-1994 V1.3    Madison 	Use ASTs on sending side, too.
**  	29-DEC-1994 V1.3-1  Madison 	LIB$ANALYZE_SDESC returns WORD length.
**  	10-JAN-1995 V1.3-2  Madison 	Reduce mailbox sizes.
**--
*/
#ifdef __DECC
#pragma module SP_MGR "V1.3-2"
#else
#ifndef __GNUC__
#module SP_MGR "V1.3-2"
#endif
#endif
#include "mmk.h"
#ifdef __GNUC__
#include <vms/iodef.h>
#include <vms/dvidef.h>
#include <vms/clidef.h>
#include <vms/jpidef.h>
#include <vms/fscndef.h>
#else
#include <iodef.h>
#include <dvidef.h>
#include <clidef.h>
#include <jpidef.h>
#include <fscndef.h>
#endif

/*
** Context block used by these routines
*/
    struct SPB {
    	struct SPB *flink, *blink;
    	struct QUE sendque;
    	unsigned short iosb[4];
    	unsigned short inchn, outchn;
    	unsigned int (*rcvast)();
    	unsigned int astprm, pid, bufsiz;
    	char *bufptr;
    	unsigned int termefn, inefn, outefn;
    	int ok_to_send;
    };

    static unsigned int spb_size = sizeof(struct SPB);

/*
**  Cell for tracking sent data
*/
    struct SPD {
    	struct SPD *flink, *blink;
    	unsigned short iosb[4];
    	unsigned short len;
    	char buf[1];
    };

/*
** Forward declarations
*/
    unsigned int sp_open(struct SPB **, struct dsc$descriptor *,
    	    	    	    unsigned int (*)(), unsigned int);
    unsigned int sp_close(struct SPB **);
    unsigned int sp_send(struct SPB **, struct dsc$descriptor *);
    unsigned int sp_receive(struct SPB **, struct dsc$descriptor *, int *);
    static unsigned int sp_wrtattn_ast(struct SPB *);
    static unsigned int sp_readattn_ast(struct SPB *);
    static unsigned int try_to_send(struct SPB *);
    static unsigned int send_completion(struct SPD *);
    static unsigned int exit_handler(unsigned int *, struct QUE *);
    static struct SPD *get_spd(int);
    static void free_spd(struct SPD *);

/*
** We keep a private queue of SPB's so we can run them down in the
** exit handler
*/
    static struct QUE spque = {&spque, &spque};
    static unsigned int exit_status, exh_declared=0;

/*
** Exit handler block
*/
    static struct EXH {
    	struct EXH *flink;
    	void *handler;
    	unsigned int argcnt;
    	void *p1, *p2;
    } exhblk = {(struct EXH *) 0, (void *) exit_handler, 2,
    	    	(void *)&exit_status, (void *) &spque};

/*
**  External references
*/
    extern int get_logical(char *, char *, int);


/*
**++
**  ROUTINE:	sp_open
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Spawns a subprocess, possibly passing it an initial command.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	sp_open(struct SPB **ctxpp, struct dsc$descriptor *inicmd,
**  	    	    unsigned int (*rcvast)(), unsigned int rcvastprm);
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    SS$_NORMAL:	    Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int sp_open(struct SPB **ctxpp, struct dsc$descriptor *inicmd,
    	    	    	unsigned int (*rcvast)(), unsigned int rcvastprm) {

    struct SPB *ctx;
    unsigned int dvi_devnam = DVI$_DEVNAM, dvi_devbufsiz = DVI$_DEVBUFSIZ;
    unsigned int spawn_flags = CLI$M_NOWAIT|CLI$M_NOKEYPAD;
    unsigned int status;
    struct dsc$descriptor inbox, outbox;

    status = lib$get_vm(&spb_size, &ctx);
    if (!OK(status)) return status;
    ctx->sendque.head = ctx->sendque.tail = &ctx->sendque;
    ctx->ok_to_send = 0;

/*
** Create the mailboxes we'll be using for I/O with the subprocess
*/
    status = sys$crembx(0, &ctx->inchn, 1024, 1024, 0xff000000, 0, 0, 0);
    if (!OK(status)) {
    	lib$free_vm(&spb_size, &ctx);
    	return status;
    }
    status = sys$crembx(0, &ctx->outchn, 1024, 1024, 0xff000000, 0, 0, 0);
    if (!OK(status)) {
    	sys$dassgn(ctx->inchn);
    	lib$free_vm(&spb_size, &ctx);
    	return status;
    }

/*
** Now that they're created, let's find out what they're called so we
** can tell LIB$SPAWN
*/
    INIT_DYNDESC(inbox);
    INIT_DYNDESC(outbox);
    lib$getdvi(&dvi_devnam, &ctx->inchn, 0, 0, &inbox);
    lib$getdvi(&dvi_devnam, &ctx->outchn, 0, 0, &outbox);
    lib$getdvi(&dvi_devbufsiz, &ctx->outchn, 0, &ctx->bufsiz);

/*
** Create the output buffer for the subprocess.
*/
    status = lib$get_vm(&ctx->bufsiz, &ctx->bufptr);
    if (!OK(status)) {
    	sys$dassgn(ctx->outchn);
    	sys$dassgn(ctx->inchn);
    	str$free1_dx(&inbox);
    	str$free1_dx(&outbox);
    	free(ctx);
    	return status;
    }

/*
** Set the "receive AST" routine to be invoked by SP_WRTATTN_AST
*/
    ctx->rcvast = rcvast;
    ctx->astprm = rcvastprm;
    sys$qiow(0, ctx->outchn, IO$_SETMODE|IO$M_WRTATTN, 0, 0, 0,
    	sp_wrtattn_ast, ctx, 0, 0, 0, 0);
    sys$qiow(0, ctx->inchn, IO$_SETMODE|IO$M_READATTN, 0, 0, 0,
    	sp_readattn_ast, ctx, 0, 0, 0, 0);

/*
** Get us a termination event flag
*/
    status = lib$get_ef(&ctx->termefn);
    if (OK(status)) lib$get_ef(&ctx->inefn);
    if (OK(status)) lib$get_ef(&ctx->outefn);
    if (!OK(status)) {
    	sys$dassgn(ctx->outchn);
    	sys$dassgn(ctx->inchn);
    	str$free1_dx(&inbox);
    	str$free1_dx(&outbox);
    	free(ctx->bufptr);
    	free(ctx);
    	return status;
    }

/*
** Now create the subprocess
*/
    status = lib$spawn(inicmd, &inbox, &outbox, &spawn_flags, 0, &ctx->pid,
    	    0, &ctx->termefn);
    if (!OK(status)) {
    	lib$free_ef(&ctx->termefn);
    	lib$free_ef(&ctx->outefn);
    	lib$free_ef(&ctx->inefn);
    	sys$dassgn(ctx->outchn);
    	sys$dassgn(ctx->inchn);
    	str$free1_dx(&inbox);
    	str$free1_dx(&outbox);
    	free(ctx->bufptr);
    	free(ctx);
    	return status;
    }

/*
** Set up the exit handler, if we haven't done so already
*/
    status = sys$setast(0);
    if (!exh_declared) {
    	sys$dclexh(&exhblk);
    	exh_declared = 1;
    }
    if (status == SS$_WASSET) sys$setast(1);

/*
** Save the SPB in our private queue
*/
    queue_insert(ctx, spque.tail);

/*
** Clean up and return
*/
    str$free1_dx(&inbox);
    str$free1_dx(&outbox);

    *ctxpp = ctx;
    return SS$_NORMAL;

} /* sp_open */

/*
**++
**  ROUTINE:	sp_close
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Close down a subprocess.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	sp_close(struct SPB **ctxpp)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    SS$_NORMAL:	    Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int sp_close(struct SPB **ctxpp) {

    struct SPB *ctx;
    struct SPD *spd;
    unsigned int status;

    ctx = *ctxpp;
/*
** Unlink the context block from our tracking queue
*/
    status = sys$setast(0);
    queue_remove(ctx, &ctx);
    while (queue_remove(ctx->sendque.head, &spd)) free_spd(spd);
    if (status == SS$_WASSET) sys$setast(1);

/*
** Delete the subprocess
*/
    sys$forcex(&ctx->pid, 0, SS$_NORMAL);
    sys$delprc(&ctx->pid, 0);

/*
** Wait till it actually dies
*/
    sys$waitfr(ctx->termefn);

/*
** Clean up and return
*/
    lib$free_ef(&ctx->termefn);
    lib$free_ef(&ctx->inefn);
    lib$free_ef(&ctx->outefn);
    sys$dassgn(ctx->inchn);
    sys$dassgn(ctx->outchn);
    lib$free_vm(&ctx->bufsiz, &ctx->bufptr);
    lib$free_vm(&spb_size, &ctx);
    return SS$_NORMAL;
} /* sp_close */

/*
**++
**  ROUTINE:	sp_send
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Queue up some data to be sent to the subprocess.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	sp_send(struct SPB **ctxpp, struct dsc$descriptor *cmdstr);
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    SS$_NORMAL:	    Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int sp_send(struct SPB **ctxpp, struct dsc$descriptor *cmdstr) {

    struct SPB *ctx;
    struct SPD *spd;
    unsigned int status, efstate;
    unsigned short cmdlen;
    char *cmdadr;

    ctx = *ctxpp;
    if (sys$readef(ctx->termefn, &efstate) != SS$_WASCLR) return SS$_NONEXPR;
    status = lib$analyze_sdesc(cmdstr, &cmdlen, &cmdadr);
    if (!OK(status)) return status;
    spd = get_spd(cmdlen);
    if (spd == 0) return SS$_INSFMEM;
    memcpy(spd->buf, cmdadr, cmdlen);
    queue_insert(spd, ctx->sendque.tail);
    sys$dclast(try_to_send, ctx, 0);

    return SS$_NORMAL;

} /* sp_send */

/*
**++
**  ROUTINE:	sp_receive
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Get some output from the subprocess, if any.  Uses IO$M_NOW modifier
**  so that if there isn't anything available, we return an error status
**  rather than blocking until something comes up.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	sp_receive(struct SPB **ctxpp, struct dsc$descriptor *rcvstr,
**  	    	    	int *rcvlen)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    SS$_NORMAL:  normal successful completion
**  	    SS$_NONEXPR: subprocess doesn't exist any more
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int sp_receive(struct SPB **ctxpp, struct dsc$descriptor *rcvstr,
    	    	    	    int *rcvlen) {

    struct SPB *ctx;
    unsigned int status, efstate;

    ctx = *ctxpp;
    if (sys$readef(ctx->termefn, &efstate) != SS$_WASCLR) return SS$_NONEXPR;

    status = sys$qiow(0, ctx->outchn, IO$_READVBLK|IO$M_NOW, ctx->iosb,
    	    	0, 0, ctx->bufptr, ctx->bufsiz, 0, 0, 0, 0);
    if (OK(status)) status = ctx->iosb[0];
    if (OK(status)) {
    	str$copy_r(rcvstr, &ctx->iosb[1], ctx->bufptr);
    	if (rcvlen) *rcvlen = ctx->iosb[1];
    }

    return status;

} /* sp_receive */

/*
**++
**  ROUTINE:	sp_wrtattn_ast
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Write-attention AST routine for the subprocess output
**  mailbox.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	sp_wrtattn_ast(struct SPB *ctx)     (at AST level)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**  	Other status values from the user-provided rcvast routine are
**  	    possible.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static unsigned int sp_wrtattn_ast(struct SPB *ctx) {

    unsigned int status;

    status = (ctx->rcvast)(ctx->astprm);
    sys$qiow(0, ctx->outchn, IO$_SETMODE|IO$M_WRTATTN, 0, 0, 0,
    	sp_wrtattn_ast, ctx, 0, 0, 0, 0);

    return status;

}

/*
**++
**  ROUTINE:	sp_readattn_ast
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Read-attention AST routine for the subprocess input
**  mailbox.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	sp_readattn_ast(struct SPB *ctx)     (at AST level)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static unsigned int sp_readattn_ast(struct SPB *ctx) {

    int i;

/*
**  Remember the old "OK to send" setting
*/
    i = ctx->ok_to_send;

/*
**  Reset it back to OK
*/
    ctx->ok_to_send = 1;
/*
**  If this was a transition, call try_to_send to see if there's data
**  waiting to go out
*/
    if (!i) try_to_send(ctx);

    return SS$_NORMAL;

} /* sp_readatnn_ast */

/*
**++
**  ROUTINE:	try_to_send
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Tries to send some data to the subprocess, if it is
**  possible to do so.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	try_to_send(struct SPB *ctx)     (at AST level)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	normal successful completion
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static unsigned int try_to_send(struct SPB *ctx) {

    struct SPD *spd;
    unsigned int status;

/*
**  Only send something if it's OK (i.e., READATTN AST has fired)
*/
    if (ctx->ok_to_send) {
/*
**  Anything to send?  If so, send it and set up the READATTN AST again.
*/
    	if (queue_remove(ctx->sendque.head, &spd)) {
    	    ctx->ok_to_send = 0;
    	    sys$qio(ctx->inefn, ctx->inchn, IO$_WRITEVBLK, spd->iosb,
    	    	send_completion, spd, spd->buf, spd->len, 0, 0, 0, 0);
    	    sys$qio(0, ctx->inchn, IO$_SETMODE|IO$M_READATTN, 0, 0, 0,
    	    	sp_readattn_ast, ctx, 0, 0, 0, 0);
    	}
    }

    return SS$_NORMAL;

} /* try_to_send */

/*
**++
**  ROUTINE:	send_completion
**
**  FUNCTIONAL DESCRIPTION:
**
**  	AST completion routine for try_to_send.  Frees up the
**  data cell that was just sent.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	send_completion(struct SPD *spd)
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

static unsigned int send_completion(struct SPD *spd) {

    free_spd(spd);

    return SS$_NORMAL;

} /* send_completion */

/*
**++
**  ROUTINE:	exit_handler
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Exit handler for the sp_mgr routines.  Closes down the
**  subprocesses and cleans up their SPB context blocks.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	exit_handler(unsigned int *stat, struct QUE *spq)
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
static unsigned int exit_handler(unsigned int *stat, struct QUE *spq) {

    struct SPB *ctx;

    while(queue_remove(spq->head, &ctx)) {
    	sys$delprc(&ctx->pid, 0);
    	lib$free_ef(&ctx->termefn);
    	lib$free_ef(&ctx->inefn);
    	lib$free_ef(&ctx->outefn);
    	sys$dassgn(ctx->inchn);
    	sys$dassgn(ctx->outchn);
    	lib$free_vm(&ctx->bufsiz, &ctx->bufptr);
    	lib$free_vm(&spb_size, &ctx);
    }

    return SS$_NORMAL;

} /* exit_handler */

/*
**++
**  ROUTINE:	sp_show_subprocess
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Uses $GETJPI to get info on the subprocess and
**  displays it � la CTRL/T.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	sp_show_subprocess(struct SPB *ctx)
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
unsigned int sp_show_subprocess(struct SPB *ctx) {

    static $DESCRIPTOR(ctrstr, "<<!AD!AD !8%T !9AD CPU=!%T PF=!UL IO=!UL MEM=!UL>>");
    static int neg10000 = -10000;
    static int zero = 0;
    struct dsc$descriptor bufdsc;
    struct {
    	unsigned short len, code;
    	char *ptr;
    	unsigned int term;
    } fscnlst = {0, FSCN$_NAME, 0, 0};
    ITMLST jpilst[8];
    char nodename[33], buf[256];
    char prcnam[16], imgnam[256];
    unsigned short prcnamlen, imgnamlen, iosb[4], buflen;
    unsigned int cputim[2], xcputim;
    unsigned int pageflts, biocnt, diocnt, wssize;
    unsigned int status;

    if (!get_logical("SYS$NODE", nodename, sizeof(nodename))) nodename[0] = '\0';
    ITMLST_INIT(jpilst[0], JPI$_PRCNAM, sizeof(prcnam), prcnam, &prcnamlen);
    ITMLST_INIT(jpilst[1], JPI$_IMAGNAME, sizeof(imgnam), imgnam, &imgnamlen);
    ITMLST_INIT(jpilst[2], JPI$_CPUTIM, sizeof(xcputim), &xcputim, 0);
    ITMLST_INIT(jpilst[3], JPI$_PAGEFLTS, sizeof(pageflts), &pageflts, 0);
    ITMLST_INIT(jpilst[4], JPI$_DIOCNT, sizeof(diocnt), &diocnt, 0);
    ITMLST_INIT(jpilst[5], JPI$_BIOCNT, sizeof(biocnt), &biocnt, 0);
    ITMLST_INIT(jpilst[6], JPI$_WSSIZE, sizeof(wssize), &wssize, 0);
    ITMLST_INIT(jpilst[7], 0, 0, 0, 0);
    status = sys$getjpiw(0, &ctx->pid, 0, jpilst, iosb, 0, 0);
    if (OK(status)) status = iosb[0];
    if (!OK(status)) return SS$_NORMAL;

    while (imgnamlen > 0 && isspace(imgnam[imgnamlen-1])) imgnamlen--;
    if (imgnamlen > 0) {
    	INIT_SDESC(bufdsc, imgnamlen, imgnam);
    	status = sys$filescan(&bufdsc, &fscnlst, 0);
    	if (!OK(status)) {
    	    fscnlst.ptr = imgnam;
    	    fscnlst.len = imgnamlen;
    	}
    } else {
    	strcpy(imgnam, "  (DCL)");
    	imgnamlen = 7;
    }
    lib$emul(&neg10000, &xcputim, &zero, cputim);
    INIT_SDESC(bufdsc, sizeof(buf), buf);
    status = sys$fao(&ctrstr, &buflen, &bufdsc, strlen(nodename), nodename,
    	prcnamlen, prcnam, 0, fscnlst.len, fscnlst.ptr,
    	cputim, pageflts, diocnt+biocnt, wssize);
    if (OK(status)) {
    	bufdsc.dsc$w_length  = buflen;
    	lib$put_output(&bufdsc);
    }

    return SS$_NORMAL;

} /* sp_show_subprocess */

/*
**++
**  ROUTINE:	get_spd
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates an SPD data cell.
**
**  RETURNS:	pointer to SPD structure.
**
**  PROTOTYPE:
**
**  	get_spd(int bufsize)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    non-0:  	Success.
**  	    0:	    	Allocation failed.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static struct SPD *get_spd(int bufsize) {

    unsigned int cellsize, status;
    struct SPD *spd;

    cellsize = bufsize + sizeof(struct SPD);

    status = lib$get_vm(&cellsize, &spd);
    if (!OK(status)) return 0;

    spd->len = bufsize;
    return spd;

} /* get_spd */

/*
**++
**  ROUTINE:	free_spd
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees an SPD data cell.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	free_spd(struct SPD *spd)
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
static void free_spd(struct SPD *spd) {

    unsigned int cellsize;

    cellsize = spd->len + sizeof(struct SPD);
    lib$free_vm(&cellsize, &spd);

    return;

} /* free_spd */
