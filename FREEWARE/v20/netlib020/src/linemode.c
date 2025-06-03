/*
**++
**  FACILITY:	NETLIB
**
**  ABSTRACT:	Line-mode send and receive routines
**
**  MODULE DESCRIPTION:
**
**  	    tbs
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1994, MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  22-OCT-1994
**
**  MODIFICATION HISTORY:
**
**  	22-OCT-1994 V1.0    Madison 	Initial coding.
**  	09-JAN-1995 V1.0-1  Madison 	Read data in 1K chunks.
**  	11-JAN-1995 V1.0-2  Madison 	Fix timeout reference in readline.
**  	13-JAN-1995 V1.0-3  Madison 	Fix split CR/LF handling in parse_out_line.
**  	25-JAN-1995 V1.0-4  Madison 	Fix for CR w/o LF.
**  	31-AUG-1995 V1.0-5  Madison 	Change writeline to write entire line,
**  	    	    	    	    	including terminator, in one write.
**  	    	    	    	    	Works around cc:Mail gateway bug.
**--
*/
#include "netlib.h"
/*
**  Forward declarations
*/
    unsigned int netlib_readline(struct CTX **xctx, struct dsc$descriptor *dsc,
    	    	    	    	unsigned short *retlen, unsigned int *flagp,
    	    	    	    	TIME *timeout, struct NETLIBIOSBDEF *iosb,
    	    	    	    	void (*astadr)(), void *astprm);
    static void readline_continue(struct IOR *ior);
    static int parse_out_line(struct CTX *ctx, struct dsc$descriptor *dsc,
    	    	    	    	unsigned short *retlen);
    unsigned int netlib_writeline(struct CTX **xctx, struct dsc$descriptor *dsc,
    	    	    	    	  struct NETLIBIOSBDEF *iosb,
    	    	    	    	  void (*astadr)(), void *astprm);
    static void writeline_finish(struct IOR *ior);
/*
**  OWN storage
*/
    static $DESCRIPTOR(crlf, "\r\n");
/*
**  External references
*/
    unsigned int netlib_read(struct CTX **xctx, void *, ...);
    unsigned int netlib_write(struct CTX **xctx, void *, ...);

/*
**++
**  ROUTINE:	netlib_readline
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
unsigned int netlib_readline (struct CTX **xctx, struct dsc$descriptor *dsc,
    	    	    	    	unsigned short *retlen, unsigned int *flagp,
    	    	    	    	TIME *timeout, struct NETLIBIOSBDEF *iosb,
    	    	    	    	void (*astadr)(), void *astprm) {

    struct CTX *ctx;
    struct NETLIBIOSBDEF myiosb;
    struct dsc$descriptor sdsc;
    unsigned int status;
    unsigned short len;
    int argc;

    VERIFY_CTX(xctx, ctx)
    SETARGCOUNT(argc);
    if (argc < 2) return SS$_INSFARG;

    if (argc < 4 || flagp == 0) ctx->line_flags = NETLIB_M_ALLOW_LF;
    else ctx->line_flags = *flagp;

    if (ctx->linebuf == 0) {
    	static unsigned int size = CTX_S_LINEBUF;
    	status = lib$get_vm(&size, &ctx->linebuf);
    	if (!OK(status)) return status;
    	ctx->line_remain = 0;
    	ctx->linebufp = ctx->lineanchor = ctx->linebuf;
    }

    len = 0;
    while (1) {

    	if (parse_out_line(ctx, dsc, &len)) {
    	    if (argc > 2 && retlen != 0) *retlen = len;
    	    status = SS$_NORMAL;
    	    break;
    	}

    	INIT_SDESC(sdsc, CTX_S_LINEBUF-(ctx->linebufp-ctx->linebuf),
    	    	    	    ctx->linebufp);
    	if (sdsc.dsc$w_length > 1024) sdsc.dsc$w_length = 1024;

    	if (argc > 6 && astadr != 0) {
    	    struct IOR *ior;
    	    GET_IOR(ior, ctx, iosb, astadr, (argc > 7) ? astprm : 0);
    	    ctx->line_retlen = retlen;
    	    ctx->line_dsc    = dsc;
    	    ctx->line_tmo    = timeout;
    	    netlib_read(xctx, &sdsc, 0, 0, 0, timeout, &ior->iosb,
    	    	    	    	readline_continue, ior);
    	    return SS$_NORMAL;
    	}

    	status = netlib_read(xctx, &sdsc, 0, 0, 0, (argc > 4) ? timeout : 0, &myiosb);
    	if (!OK(status)) break;
    	ctx->line_remain = myiosb.iosb_w_count;

    } /* while */

    if (argc > 2 && retlen != 0) *retlen = len;

    if (argc > 5 && iosb != 0) {
    	iosb->iosb_w_status = status;
    	iosb->iosb_w_count = len;
    }

    if (argc > 6 && astadr != 0) {
    	sys$dclast(astadr, (argc > 7) ? astprm : 0, 0);
    	return SS$_NORMAL;
    }

    return status;

} /* netlib_readline */

/*
**++
**  ROUTINE:	readline_continue
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
static void readline_continue (struct IOR *ior) {

    struct CTX *ctx = ior->ctx;
    struct dsc$descriptor sdsc;
    unsigned int status;
    unsigned short len;

    if (!OK(ior->iosb.iosb_w_status)) {
    	if (ior->iosbp != 0) {
    	    ior->iosbp->iosb_w_status =
    	    	ior->iosb.iosb_w_status;
    	    ior->iosbp->iosb_w_count = 0;
    	}
    	if (ctx->line_retlen != 0) *(ctx->line_retlen) = 0;
    	if (ior->astadr != 0) (*ior->astadr)(ior->astprm);
    	FREE_IOR(ior);
    	return;
    }

    ctx->line_remain = ior->iosb.iosb_w_count;

    if (parse_out_line(ctx, ctx->line_dsc, &len)) {
    	if (ior->iosbp != 0) {
    	    ior->iosbp->iosb_w_status = SS$_NORMAL;
    	    ior->iosbp->iosb_w_count = len;
    	}
    	if (ctx->line_retlen != 0) *(ctx->line_retlen) = len;
    	if (ior->astadr != 0) (*ior->astadr)(ior->astprm);
    	FREE_IOR(ior);
    	return;
    }
    	
    INIT_SDESC(sdsc, CTX_S_LINEBUF-(ctx->linebufp-ctx->linebuf), ctx->linebufp);
    if (sdsc.dsc$w_length > 1024) sdsc.dsc$w_length = 1024;
    netlib_read(&ctx, &sdsc, 0, 0, 0, ctx->line_tmo, &ior->iosb,
    	    	    	    	readline_continue, ior);

    return;

} /* readline_continue */

/*
**++
**  ROUTINE:	parse_out_line
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
static int parse_out_line (struct CTX *ctx, struct dsc$descriptor *dsc,
    	    	    	    	unsigned short *retlen) {

    unsigned short len;
    unsigned char *cp;

    while (ctx->line_remain > 0) {
    	if (ctx->flags & CTX_M_LINE_FOUND_CR) {
    	    ctx->flags &= ~CTX_M_LINE_FOUND_CR;
    	    if (*ctx->linebufp == '\n') {
    	    	len = (ctx->linebufp - ctx->lineanchor) - 1;
    	    	str$copy_r(dsc, &len, ctx->lineanchor);
    	    	*retlen = len;
    	    	ctx->line_remain -= 1;
    	    	ctx->linebufp += 1;
    	    	ctx->lineanchor = ctx->linebufp;
    	    	return 1;
    	    }
    	}

    	cp = memchr(ctx->linebufp, '\r', ctx->line_remain);
    	if (cp == 0) {
    	    if (ctx->line_flags & NETLIB_M_ALLOW_LF) {
    	    	cp = memchr(ctx->linebufp, '\n', ctx->line_remain);
    	    	if (cp != 0) {
    	    	    len = cp - ctx->lineanchor;
    	    	    str$copy_r(dsc, &len, ctx->lineanchor);
    	    	    *retlen = len;
    	    	    len -= (ctx->linebufp-ctx->lineanchor);
    	    	    ctx->line_remain -= len + 1;
    	    	    ctx->linebufp += len + 1;
    	    	    ctx->lineanchor = ctx->linebufp;
    	    	    return 1;
    	    	}
    	    }
    	    ctx->linebufp += ctx->line_remain;
    	    ctx->line_remain = 0;
    	} else {
    	    if ((cp - ctx->linebufp) == (ctx->line_remain-1)) {
    	    	ctx->flags |= CTX_M_LINE_FOUND_CR;
    	    	ctx->linebufp += ctx->line_remain;
    	    	ctx->line_remain = 0;
    	    } else {
    	    	if (*(cp + 1) == '\n') {
    	    	    len = cp - ctx->lineanchor;
    	    	    str$copy_r(dsc, &len, ctx->lineanchor);
    	    	    *retlen = len;
    	    	    len -= (ctx->linebufp-ctx->lineanchor);
    	    	    ctx->line_remain -= len + 2;
    	    	    ctx->linebufp += len + 2;
    	    	    ctx->lineanchor = ctx->linebufp;
    	    	    return 1;
    	    	} else {
    	    	    ctx->line_remain -= (cp - ctx->linebufp) + 1;
    	    	    ctx->linebufp = cp + 1;
    	    	}
    	    }
    	}
    }

    if (ctx->linebufp - ctx->lineanchor >= CTX_S_LINEBUF-2) {
    	len = ctx->linebufp - ctx->lineanchor;
    	str$copy_r(dsc, &len, ctx->lineanchor);
    	ctx->line_remain = 0;
    	ctx->lineanchor = ctx->linebufp = ctx->linebuf;
    	*retlen = len;
    	return 1;
    }

    if (ctx->line_remain > 0 ||
    	    (ctx->linebufp - ctx->linebuf == CTX_S_LINEBUF-ctx->line_remain)) {
    	len = ctx->line_remain + ctx->linebufp-ctx->lineanchor;
    	memmove(ctx->linebuf, ctx->lineanchor, len);
    	ctx->linebufp = ctx->linebuf + len;
    	ctx->lineanchor = ctx->linebuf;
    	ctx->line_remain = 0;
    }
    return 0;

} /* parse_out_line */

/*
**++
**  ROUTINE:	netlib_writeline
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
unsigned int netlib_writeline (struct CTX **xctx, struct dsc$descriptor *dsc,
    	    	    	    	  struct NETLIBIOSBDEF *iosb,
    	    	    	    	  void (*astadr)(), void *astprm) {

    struct CTX *ctx;
    struct IOR *ior;
    struct NETLIBIOSBDEF myiosb;
    struct dsc$descriptor sdsc;
    char *line, *bufptr;
    unsigned short len, buflen;
    unsigned int status, count;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (argc < 2) return SS$_INSFARG;
    if (dsc == 0) return SS$_BADPARAM;
    status = lib$analyze_sdesc(dsc, &buflen, &bufptr);
    if (!OK(status)) return status;
/*
**  Make sure our write buffer is large enough to handle
**  the entire string plus the terminating CRLF.
*/
    if (buflen+2 > ctx->wlinesize) {
    	if (ctx->wlinebuf != 0) {
    	    lib$free_vm(&ctx->wlinesize, &ctx->wlinebuf);
    	    ctx->wlinebuf = 0;
    	    ctx->wlinesize = 0;
    	}
    	ctx->wlinesize = ((buflen < 2046) ? 2046 : buflen) + 2;
    	status = lib$get_vm(&ctx->wlinesize, &ctx->wlinebuf);
    	if (!OK(status)) {
    	    ctx->wlinesize = 0;
    	    ctx->wlinebuf = 0;
    	    return status;
    	}
    }

/*
**  Now copy the user's data into our buffer and tack on the
**  terminating CRLF.
**
**  We do this because some applications are broken and assume
**  that one entire line can be read in in a single read, rather
**  than actually parsing the stream of data coming in for the
**  line termination sequence.  Sigh.
*/
    memcpy(ctx->wlinebuf, bufptr, buflen);
    ctx->wlinebuf[buflen++] = '\r';
    ctx->wlinebuf[buflen++] = '\n';

    INIT_SDESC(sdsc, buflen, ctx->wlinebuf);

    if (argc > 3 && astadr != 0) {
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 4) ? astprm : 0);
    	return netlib_write(xctx, &sdsc, 0, 0, &ior->iosb,
    	    	    writeline_finish, ior);
    }

    status = netlib_write(xctx, &sdsc, 0, 0, &myiosb);
    if (argc > 2 && iosb != 0) {
    	iosb->iosb_w_status = myiosb.iosb_w_status;
    	iosb->iosb_w_count = myiosb.iosb_w_count;
    	iosb->iosb_l_unused = 0;
    }

    return status;

} /* netlib_writeline */

/*
**++
**  ROUTINE:	writeline_finish
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
static void writeline_finish (struct IOR *ior) {

    if (ior->iosbp != 0) {
    	ior->iosbp->iosb_w_status = ior->iosb.iosb_w_status;
    	ior->iosbp->iosb_w_count  = ior->iosb.iosb_w_count;
    }

    if (ior->astadr != 0) (*ior->astadr)(ior->astprm);

    FREE_IOR(ior);

} /* writeline_finish */

