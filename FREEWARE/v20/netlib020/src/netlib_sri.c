/*
**++
**  FACILITY:	NETLIB
**
**  ABSTRACT:	Routines specific to the SRI-QIO interface
**  	    	(MultiNet, TCPware, Pathway).
**
**  MODULE DESCRIPTION:
**
**  	tbs
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1994, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  20-OCT-1994
**
**  MODIFICATION HISTORY:
**
**  	20-OCT-1994 V1.0    Madison 	Initial coding.
**  	17-NOV-1994 V1.0-1  Madison 	Wasn't mapping 0-length read to LINKDISCON.
**  	08-JUN-1995 V1.0-2  Madison 	Don't wipe out COPY_FROM flag with TIMED.
**  	13-JUN-1995 V1.0-3  Madison 	Second try at the COPY_FROM fix.
**  	13-OCT-1995 V1.0-4  Madison 	Free IOR's on $QIO failure.
**--
*/
#include "netlib_sri.h"
#include "netlib.h"
/*
**  Forward declarations
*/
    unsigned int netlib_socket(struct CTX **xctx, unsigned int *type,
    	    	    	    	unsigned int *family);
    unsigned int netlib_server_setup(struct CTX **xctx,
    	    	    	    struct SINDEF *sa, unsigned int *salen);
    unsigned int netlib_bind(struct CTX **xctx, struct SINDEF *sa,
    	    	    	    unsigned int *salen, struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm);
    static unsigned int io_completion(struct IOR *ior);
    unsigned int netlib_getsockname(struct CTX **ctx, struct SINDEF *sa,
    	    	    	    	 unsigned int *sasize, unsigned int *salen,
    	    	    	    	 struct NETLIBIOSBDEF *iosb,
    	    	    	    	 void (*astadr)(), void *astprm);
    unsigned int netlib_getpeername(struct CTX **ctx, struct SINDEF *sa,
    	    	    	    	 unsigned int *sasize, unsigned int *salen,
    	    	    	    	 struct NETLIBIOSBDEF *iosb,
    	    	    	    	 void (*astadr)(), void *astprm);
    unsigned int netlib_connect(struct CTX **ctx, struct SINDEF *sa,
    	    	    	    	 unsigned int *salen,
    	    	    	    	 struct NETLIBIOSBDEF *iosb,
    	    	    	    	 void (*astadr)(), void *astprm);
    unsigned int netlib_write(struct CTX **ctx, struct dsc$descriptor *dsc,
    	    	    	    struct SINDEF *sa, unsigned int *salen,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm);
    unsigned int netlib_read(struct CTX **ctx, struct dsc$descriptor *dsc,
    	    	    	    struct SINDEF *sa, unsigned int *sasize,
    	    	    	    unsigned int *salen, TIME *tmo,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm);
    static unsigned int io_timeout(struct IOR *ior);
    unsigned int netlib_shutdown(struct CTX **xctx, unsigned int *shuttype,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm);
    unsigned int netlib_close(struct CTX **xctx);
    unsigned int netlib_listen(struct CTX **xctx, unsigned int *backlog,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm);
    unsigned int netlib_accept(struct CTX **ctx, struct CTX **xnewctx,
    	    	    	    struct SINDEF *sa, unsigned int *sasize,
    	    	    	    unsigned int *salen, struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm);
    unsigned int netlib_setsockopt(struct CTX **xctx,
    	    	    	    unsigned int *level, unsigned int *option,
    	    	    	    void *value, unsigned int *vallen,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm);
    unsigned int netlib_getsockopt(struct CTX **xctx,
    	    	    	    unsigned int *level, unsigned int *option,
    	    	    	    void *value, unsigned int *valsize,
    	    	    	    unsigned int *vallen,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm);
    unsigned int netlib_name_to_address(struct CTX **xctx, unsigned int *whichp,
    	    	    	    struct dsc$descriptor *namdsc,
    	    	    	    struct INADDRDEF *addrlist,
    	    	    	    unsigned int *listsize, unsigned int *count,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm);
    unsigned int netlib_address_to_name(struct CTX **xctx, unsigned int *whichp,
    	    	    	    struct INADDRDEF *addr,
    	    	    	    unsigned int *addrsize,
    	    	    	    struct dsc$descriptor *namdsc,
    	    	    	    unsigned short *retlen,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm);
    static unsigned int netlib___cvt_status(struct NETLIBIOSBDEF *iosb);
    static void netlib___cvt_iosb(struct NETLIBIOSBDEF *dst, struct NETLIBIOSBDEF *src);
    unsigned int netlib_dns_mx_lookup(struct CTX **xctx,
    	    	    struct dsc$descriptor *namdsc, struct MXRRDEF *mxrr,
    	    	    unsigned int *mxrrsize, unsigned int *mxrrcount,
    	    	    struct NETLIBIOSBDEF *iosb, void (*astadr)(), void *astprm);
/*
**  OWN storage
*/
    static $DESCRIPTOR(inetdevice, "_INET0:");
    static unsigned short errorvec[] = {
    	SS$_ABORT, SS$_ABORT, SS$_NOSUCHNODE, SS$_ABORT, SS$_ABORT,
    	SS$_NOSUCHNODE, SS$_ABORT, SS$_ABORT, SS$_BADPARAM, SS$_ABORT,
    	SS$_ABORT, SS$_INSFMEM, SS$_NOPRIV, SS$_ACCVIO, SS$_ABORT,
    	SS$_ABORT, SS$_FILALRACC, SS$_ABORT, SS$_ABORT, SS$_ABORT,
    	SS$_ABORT, SS$_BADPARAM, SS$_ABORT, SS$_ABORT, SS$_ABORT,
    	SS$_ABORT, SS$_ABORT, SS$_ABORT, SS$_ABORT, SS$_ABORT,
    	SS$_ABORT, SS$_LINKDISCON, SS$_BADPARAM, SS$_TOOMUCHDATA, SS$_SUSPENDED,
    	SS$_ABORT, SS$_ABORT, SS$_NOTNETDEV, SS$_NOSUCHNODE, SS$_TOOMUCHDATA,
    	SS$_PROTOCOL, SS$_PROTOCOL, SS$_PROTOCOL, SS$_PROTOCOL, SS$_ILLCNTRFUNC,
    	SS$_PROTOCOL, SS$_PROTOCOL, SS$_DUPLNAM, SS$_IVADDR, SS$_UNREACHABLE,
    	SS$_UNREACHABLE, SS$_RESET, SS$_LINKABORT, SS$_CONNECFAIL, SS$_INSFMEM,
    	SS$_FILALRACC, SS$_NOLINKS, SS$_SHUT, SS$_ABORT, SS$_TIMEOUT,
    	SS$_REJECT, SS$_ABORT, SS$_ABORT, SS$_SHUT, SS$_UNREACHABLE};
/*
**  External references
*/
    unsigned int netlib___dns_name_to_addr(struct CTX *ctx,
    	    	    struct dsc$descriptor *namdsc, struct INADDRDEF *addrlist,
    	    	    unsigned int listsize, unsigned int *count,
    	    	    struct NETLIBIOSBDEF *iosb, void (*astadr)(), void *astprm);
    unsigned int netlib___ht_name_to_addr(struct CTX *ctx,
    	    	    struct dsc$descriptor *namdsc, struct INADDRDEF *addrlist,
    	    	    unsigned int listsize, unsigned int *count,
    	    	    struct NETLIBIOSBDEF *iosb, void (*astadr)(), void *astprm);
    unsigned int netlib___dns_addr_to_name(struct CTX *ctx,
    	    	    struct INADDRDEF *addrlist, unsigned int addrsize,
    	    	    struct dsc$descriptor *namdsc, unsigned short *retlen,
    	    	    struct NETLIBIOSBDEF *iosb, void (*astadr)(), void *astprm);
    unsigned int netlib___ht_addr_to_name(struct CTX *ctx,
    	    	    struct INADDRDEF *addrlist, unsigned int addrsize,
    	    	    struct dsc$descriptor *namdsc, unsigned short *retlen,
    	    	    struct NETLIBIOSBDEF *iosb, void (*astadr)(), void *astprm);
    unsigned int netlib_strtoaddr(struct dsc$descriptor *, struct INADDRDEF *);
    unsigned int netlib___dns_mx_lookup(struct CTX **xctx,
    	    	    struct dsc$descriptor *namdsc, struct MXRRDEF *mxrr,
    	    	    unsigned int *mxrrsize, unsigned int *mxrrcount,
    	    	    struct NETLIBIOSBDEF *iosb, void (*astadr)(), void *astprm);

/*
**++
**  ROUTINE:	netlib_socket
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Create a "socket".
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
unsigned int netlib_socket (struct CTX **xctx, unsigned int *type,
    	    	    	    	unsigned int *family) {

    struct CTX *ctx;
    struct NETLIBIOSBDEF myiosb;
    unsigned int status, af, ty;
    int argc;

    SETARGCOUNT(argc);

    if (argc < 1) return SS$_INSFARG;

    status = netlib___alloc_ctx(&ctx, SPECCTX_SIZE);
    if (!OK(status)) return status;

    status = sys$assign(&inetdevice, &ctx->chan, 0, 0);
    if (!OK(status)) {
    	netlib___free_ctx(ctx);
    	return status;
    }

    ty = (argc >= 2 && type != 0) ? *type : NETLIB_K_TYPE_STREAM;
    af = (argc >= 3 && family != 0) ? *family : NETLIB_K_AF_INET;

    status = sys$qiow(netlib_synch_efn, ctx->chan, IO$_SOCKET,
    	    	    	&myiosb, 0, 0,
    	    	    	af, ty, 0, 0, 0, 0);
    if (OK(status)) status = netlib___cvt_status(&myiosb);

    if (!OK(status)) {
    	sys$dassgn(ctx->chan);
    	netlib___free_ctx(ctx);
    	return status;
    }

    *xctx = ctx;
    return SS$_NORMAL;

} /* netlib_socket */

/*
**++
**  ROUTINE:	netlib_server_setup
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Create a "server_setup".
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
unsigned int netlib_server_setup (struct CTX **xctx, struct SINDEF *sa,
    	    	    	    	    unsigned int *salen) {

    struct CTX *ctx;
    unsigned int status;

    static $DESCRIPTOR(device, "SYS$INPUT");

    status = netlib___alloc_ctx(&ctx, SPECCTX_SIZE);
    if (!OK(status)) return status;

    status = sys$assign(&device, &ctx->chan, 0, 0);
    if (!OK(status)) {
    	netlib___free_ctx(ctx);
    	return status;
    }

    *xctx = ctx;
    return SS$_NORMAL;

} /* netlib_server_setup */

/*
**++
**  ROUTINE:	netlib_bind
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
unsigned int netlib_bind (struct CTX **xctx, struct SINDEF *sa,
    	    	    	    unsigned int *salen, struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm) {

    struct CTX *ctx;
    struct NETLIBIOSBDEF myiosb;
    unsigned int status;
    int argc;
    static unsigned int one = 1;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (sa == 0 || salen == 0) return SS$_BADPARAM;
    if (*salen < sizeof(struct SINDEF)) return SS$_BADPARAM;

    if (!(ctx->flags & CTX_M_USER_SET_REUSEADDR)) {
    	sys$qiow(netlib_synch_efn, ctx->chan, IO$_SETSOCKOPT, 0, 0, 0,
    	    	    NETLIB_K_LEVEL_SOCKET, NETLIB_K_OPTION_REUSEADDR,
    	    	    &one, sizeof(one), 0, 0);
    }

    if (argc > 4 && astadr != 0) {
    	struct IOR *ior;
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 5) ? astprm : 0);
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO$_BIND, &ior->iosb,
    	    	    	    io_completion, ior, sa, *salen, 0, 0, 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO$_BIND, &myiosb,
    	    	    	    0, 0, sa, *salen, 0, 0, 0, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 3 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    }

    return status;

} /* netlib_bind */

/*
**++
**  ROUTINE:	io_completion
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
static unsigned int io_completion (struct IOR *ior) {

    struct CTX *ctx;

    ctx = ior->ctx;

    ior->iorflags |= IOR_M_IO_COMPLETED;

    if (ior->iorflags & IOR_M_IO_TIMED) sys$cantim(ior, 0);

    if (ior->iosb.iosb_w_status == SS$_CANCEL)
    	ior->iosb.iosb_w_status = SS$_TIMEOUT;

    if (ior->iorflags & IOR_M_CHECK_LENGTH &&
    	    OK(ior->iosb.iosb_w_status) && ior->iosb.iosb_w_count == 0)
    	ior->iosb.iosb_w_status = SS$_LINKDISCON;

    if (ior->iosbp != 0) netlib___cvt_iosb(ior->iosbp, &ior->iosb);

    if (ior->iorflags & IOR_M_COPY_LENGTH) {
    	if (OK(ior->iosb.iosb_w_status) && ior->spec_retlen != 0)
    	    	*(unsigned int *) ior->spec_retlen = ior->spec_length;
    	ior->iorflags &= ~IOR_M_COPY_LENGTH;
    }

    if (ior->iorflags & IOR_M_COPY_FROM) {
    	if (OK(ior->iosb.iosb_w_status) && ior->spec_userfrom != 0) {
    	    unsigned int len;
    	    len = ior->specior.from.length;
    	    if (len > ior->spec_length) len = ior->spec_length;
    	    memcpy(ior->spec_userfrom, &ior->specior.from.address, len);
    	    if (ior->spec_retlen != 0) *(unsigned int *)ior->spec_retlen = len;
    	}
    	ior->iorflags &= ~IOR_M_COPY_FROM;
    }

    if (ior->iorflags & IOR_M_COPY_FROM2) {
    	if (OK(ior->iosb.iosb_w_status) && ior->spec_userfrom != 0) {
    	    unsigned int len;
    	    len = ior->specior.from2.length;
    	    if (len > ior->spec_length) len = ior->spec_length;
    	    memcpy(ior->spec_userfrom, &ior->specior.from2.address, len);
    	    if (ior->spec_retlen != 0) *(unsigned int *)ior->spec_retlen = len;
    	}
    	ior->iorflags &= ~IOR_M_COPY_FROM2;
    }

    if (ior->iorflags & IOR_M_NEW_CONTEXT) {
    	if (OK(ior->iosb.iosb_w_status)) {
    	    *((struct CTX **) ior->spec_xnewctx) = (struct CTX *) ior->spec_newctx;
    	} else {
    	    sys$dassgn(((struct CTX *) ior->spec_newctx)->chan);
    	    netlib___free_ctx((struct CTX *) ior->spec_newctx);
    	}
    	ior->iorflags &= ~IOR_M_NEW_CONTEXT;
    }

    if (ior->astadr != 0) (*(ior->astadr))(ior->astprm);

    FREE_IOR(ior);

    return SS$_NORMAL;

} /* io_completion */

/*
**++
**  ROUTINE:	netlib_getsockname
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
unsigned int netlib_getsockname (struct CTX **xctx, struct SINDEF *sa,
    	    	    	    	 unsigned int *sasize, unsigned int *salen,
    	    	    	    	 struct NETLIBIOSBDEF *iosb,
    	    	    	    	 void (*astadr)(), void *astprm) {
    struct CTX *ctx;
    unsigned int status;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (sa == 0 || sasize == 0) return SS$_BADPARAM;

    if (argc > 5 && astadr != 0) {
    	struct IOR *ior;
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 6) ? astprm : 0);
    	ior->spec_length = *sasize;
    	ior->spec_retlen = salen;
    	ior->iorflags = IOR_M_COPY_LENGTH;
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO$_GETSOCKNAME, &ior->iosb,
    	    	    	    io_completion, ior, sa, &ior->spec_length,
    	    	    	    0, 0, 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	unsigned int length;
    	length = *sasize;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO$_GETSOCKNAME,
    	    	    	    &myiosb,
    	    	    	    0, 0, sa, &length, 0, 0, 0, 0);
    	if (argc > 3 && salen != 0) *salen = length;
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 4 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    }

    return status;

} /* netlib_getsockname */

/*
**++
**  ROUTINE:	netlib_getpeername
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
unsigned int netlib_getpeername (struct CTX **xctx, struct SINDEF *sa,
    	    	    	    	 unsigned int *sasize, unsigned int *salen,
    	    	    	    	 struct NETLIBIOSBDEF *iosb,
    	    	    	    	 void (*astadr)(), void *astprm) {
    struct CTX *ctx;
    unsigned int status;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (sa == 0 || sasize == 0) return SS$_BADPARAM;

    if (argc > 5 && astadr != 0) {
    	struct IOR *ior;
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 6) ? astprm : 0);
    	ior->spec_length = *sasize;
    	ior->spec_retlen = salen;
    	ior->iorflags = IOR_M_COPY_LENGTH;
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO$_GETPEERNAME, &ior->iosb,
    	    	    	    io_completion, ior, sa, &ior->spec_length,
    	    	    	    0, 0, 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	unsigned int length;
    	length = *sasize;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO$_GETPEERNAME,
    	    	    	    &myiosb,
    	    	    	    0, 0, sa, &length, 0, 0, 0, 0);
    	if (argc > 3 && salen != 0) *salen = length;
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 4 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    }

    return status;

} /* netlib_getpeername */

/*
**++
**  ROUTINE:	netlib_connect
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
unsigned int netlib_connect (struct CTX **xctx, struct SINDEF *sa,
    	    	    	    	 unsigned int *salen,
    	    	    	    	 struct NETLIBIOSBDEF *iosb,
    	    	    	    	 void (*astadr)(), void *astprm) {
    struct CTX *ctx;
    unsigned int status;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (sa == 0 || salen == 0) return SS$_BADPARAM;
    if (*salen < sizeof(struct SINDEF)) return SS$_BADPARAM;

    if (argc > 4 && astadr != 0) {
    	struct IOR *ior;
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 5) ? astprm : 0);
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO$_CONNECT, &ior->iosb,
    	    	    	    io_completion, ior, sa, *salen,
    	    	    	    0, 0, 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO$_CONNECT, &myiosb,
    	    	    	    0, 0, sa, *salen, 0, 0, 0, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 3 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    }

    return status;

} /* netlib_connect */

/*
**++
**  ROUTINE:	netlib_write
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
unsigned int netlib_write (struct CTX **xctx, struct dsc$descriptor *dsc,
    	    	    	    struct SINDEF *sa, unsigned int *salen,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm) {
    struct CTX *ctx;
    void *bufptr;
    unsigned int status;
    unsigned short buflen;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    status = lib$analyze_sdesc(dsc, &buflen, &bufptr);
    if (!OK(status)) return status;

    if (argc > 5 && astadr != 0) {
    	struct IOR *ior;
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 6) ? astprm : 0);
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO$_WRITEVBLK, &ior->iosb,
    	    	    	    io_completion, ior, bufptr, buflen, 0, sa,
    	    	    	    (salen == 0) ? 0 : *salen,
    	    	    	    0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO$_WRITEVBLK,
    	    	    	    &myiosb, 0, 0, bufptr, buflen, 0,
    	    	    	    (argc > 2) ? sa : 0,
    	    	    	    (argc > 3 && salen != 0) ? *salen : 0, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 4 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    }

    return status;

} /* netlib_write */

/*
**++
**  ROUTINE:	netlib_read
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
unsigned int netlib_read (struct CTX **xctx, struct dsc$descriptor *dsc,
    	    	    	    struct SINDEF *sa, unsigned int *sasize,
    	    	    	    unsigned int *salen, TIME *timeout,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm) {
    struct CTX *ctx;
    unsigned int status;
    struct IOR *ior;
    int argc, do_from;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (dsc->dsc$b_dtype != DSC$K_DTYPE_T && dsc->dsc$b_dtype != 0) {
    	return SS$_BADPARAM;
    }

    if (dsc->dsc$b_class != DSC$K_CLASS_S && dsc->dsc$b_class != 0) {
    	return SS$_BADPARAM;
    }

    do_from = (argc > 3 && sa != 0 && sasize != 0 && *sasize != 0);
    if (argc > 7 && astadr != 0) {
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 8) ? astprm : 0);
    	if (do_from) {
    	    ior->specior.from.length = sizeof(ior->specior.from.address);
    	    ior->spec_userfrom = sa;
    	    ior->spec_length = *sasize;
    	    ior->spec_retlen = salen;
    	    ior->iorflags = IOR_M_COPY_FROM;
    	} else ior->iorflags = 0;

    	if (timeout != 0) {
    	    ior->iorflags |= IOR_M_IO_TIMED;
    	    status = sys$setimr(netlib_asynch_efn, timeout, io_timeout, ior);
    	    if (!OK(status)) {
    	    	FREE_IOR(ior);
    	    	return status;
    	    }
    	}
    	ior->iorflags |= IOR_M_CHECK_LENGTH;
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO$_READVBLK, &ior->iosb,
    	    	    	    io_completion, ior, dsc->dsc$a_pointer,
    	    	    	    dsc->dsc$w_length, 0,
    	    	    	    do_from ? &ior->specior.from : 0,
    	    	    	    do_from ? sizeof(ior->specior.from) : 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct {
    	    unsigned short length;
    	    struct SINDEF address;
    	} from;
    	struct NETLIBIOSBDEF myiosb;

    	from.length = sizeof(from.address);
    	if (argc > 5 && timeout != 0) {
    	    GET_IOR(ior, ctx, 0, 0, 0);
    	    ior->iorflags = IOR_M_IO_TIMED;
    	    status = sys$setimr(netlib_asynch_efn, timeout, io_timeout, ior);
    	    if (!OK(status)) {
    	    	FREE_IOR(ior);
    	    	return status;
    	    }
    	}
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO$_READVBLK,
    	    	    	    &myiosb, 0, 0,
    	    	    	    dsc->dsc$a_pointer, dsc->dsc$w_length, 0,
    	    	    	    do_from ? &from : 0,
    	    	    	    do_from ? sizeof(from) : 0, 0);
    	if (argc > 5 && timeout != 0) {
    	    sys$cantim(ior, 0);
    	    FREE_IOR(ior);
    	}

    	if (OK(status)) {
    	    status = netlib___cvt_status(&myiosb);
    	    if (OK(status) && myiosb.iosb_w_count == 0)
    	    	status = myiosb.iosb_w_status = SS$_LINKDISCON;
    	}
    	if (argc > 6 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    	if (OK(status) && do_from) {
    	    unsigned int len;
    	    len = from.length;
    	    if (len > *sasize) len = *sasize;
    	    memcpy(sa, &from.address, len);
    	    if (argc > 4 && salen != 0) *salen = len;
    	}
    }

    return status;

} /* netlib_read */

/*
**++
**  ROUTINE:	io_timeout
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
static unsigned int io_timeout (struct IOR *ior) {

    if (ior->iorflags & IOR_M_IO_COMPLETED) return SS$_NORMAL;
    return sys$cancel(ior->ctx->chan);

} /* io_timeout */

/*
**++
**  ROUTINE:	netlib_shutdown
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
unsigned int netlib_shutdown (struct CTX **xctx, unsigned int *shuttype,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm) {

    struct CTX *ctx;
    unsigned int status;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (argc > 3 && astadr != 0) {
    	struct IOR *ior;
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 5) ? astprm : 0);
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO$_SHUTDOWN, &ior->iosb,
    	    	    	    io_completion, ior,
    	    	    	    (shuttype == 0) ? 0 : *shuttype, 0, 0, 0, 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO$_SHUTDOWN, &myiosb,
    	    	    	    0, 0, (argc > 1 && shuttype != 0) ? *shuttype : 0,
    	    	    	    0, 0, 0, 0, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 2 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    }

    return status;

} /* netlib_shutdown */

/*
**++
**  ROUTINE:	netlib_close
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
unsigned int netlib_close (struct CTX **xctx) {

    struct CTX *ctx;
    unsigned int status;

    VERIFY_CTX(xctx, ctx);

    status = sys$dassgn(ctx->chan);
    if (!OK(status)) return status;

    netlib___free_ctx(ctx);

    *xctx = 0;

    return SS$_NORMAL;

} /* netlib_close */

/*
**++
**  ROUTINE:	netlib_listen
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
unsigned int netlib_listen (struct CTX **xctx, unsigned int *backlog,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm) {

    struct CTX *ctx;
    unsigned int status;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (argc > 3 && astadr != 0) {
    	struct IOR *ior;
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 4) ? astprm : 0);
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO$_LISTEN, &ior->iosb,
    	    	    	    io_completion, ior,
    	    	    	    (backlog == 0) ? 0 : *backlog, 0, 0, 0, 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO$_LISTEN, &myiosb,
    	    	    	    0, 0, (argc > 1 && backlog != 0) ? *backlog : 0,
    	    	    	    0, 0, 0, 0, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 2 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    }

    return status;

} /* netlib_listen */

/*
**++
**  ROUTINE:	netlib_accept
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
unsigned int netlib_accept (struct CTX **xctx, struct CTX **xnewctx,
    	    	    	    struct SINDEF *sa, unsigned int *sasize,
    	    	    	    unsigned int *salen, struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm) {
    struct CTX *ctx, *newctx;
    unsigned int status;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (argc < 2) return SS$_INSFARG;
    if (xnewctx == 0) return SS$_BADPARAM;
    status = netlib___alloc_ctx(&newctx, SPECCTX_SIZE);
    if (!OK(status)) return status;

    status = sys$assign(&inetdevice, &newctx->chan, 0, 0);
    if (!OK(status)) {
    	netlib___free_ctx(newctx);
    	return status;
    }

    if (argc > 6 && astadr != 0) {
    	struct IOR *ior;
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 7) ? astprm : 0);
    	ior->specior.from2.length = sizeof(ior->specior.from2.address);
    	ior->spec_userfrom = (sasize == 0) ? 0 : sa;
    	ior->spec_length = (sasize == 0) ? 0 : *sasize;
    	ior->spec_retlen = salen;
    	ior->spec_xnewctx = xnewctx;
    	ior->spec_newctx = newctx;
    	ior->iorflags = IOR_M_NEW_CONTEXT;
    	if (ior->spec_userfrom != 0)
    	    ior->iorflags |= IOR_M_COPY_FROM2;
    	status = sys$qio(netlib_asynch_efn, newctx->chan, IO$_ACCEPT, &ior->iosb,
    	    	    	    io_completion, ior,
    	    	    	    &ior->specior.from2, sizeof(ior->specior.from2),
    	    	    	    ctx->chan, 0, 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct {
    	    unsigned int length;
    	    struct SINDEF address;
    	} from;
    	struct NETLIBIOSBDEF myiosb;
    	from.length = sizeof(from.address);
    	status = sys$qiow(netlib_synch_efn, newctx->chan, IO$_ACCEPT,
    	    	    	    &myiosb, 0, 0,
    	    	    	    &from, sizeof(from),
    	    	    	    ctx->chan, 0, 0, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 5 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    	if (OK(status)) {
    	    *xnewctx = newctx;
    	    if (argc > 3 && sa != 0 && sasize != 0) {
    	    	unsigned int len;
    	    	len = from.length;
    	    	if (len > *sasize) len = *sasize;
    	    	memcpy(sa, &from.address, len);
    	    	if (argc > 4 && salen != 0) *salen = len;
    	    }
    	}
    }

    return status;

} /* netlib_accept */

/*
**++
**  ROUTINE:	netlib_setsockopt
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
unsigned int netlib_setsockopt (struct CTX **xctx,
    	    	    	    unsigned int *level, unsigned int *option,
    	    	    	    void *value, unsigned int *vallen,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm) {

    struct CTX *ctx;
    unsigned int status, lev;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (argc < 5) return SS$_INSFARG;
    if (option == 0 || value == 0 || vallen == 0) return SS$_BADPARAM;
    if (level == 0) lev = NETLIB_K_LEVEL_SOCKET;
    else lev = *level;

    if (lev == NETLIB_K_LEVEL_SOCKET && *option == NETLIB_K_OPTION_REUSEADDR) {
    	ctx->flags |= CTX_M_USER_SET_REUSEADDR;
    }

    if (argc > 6 && astadr != 0) {
    	struct IOR *ior;
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 7) ? astprm : 0);
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO$_SETSOCKOPT, &ior->iosb,
    	    	    	    io_completion, ior, lev, *option,
    	    	    	    value, *vallen, 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO$_SETSOCKOPT,
    	    	    	    &myiosb, 0, 0, lev, *option, value, *vallen, 0, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 5 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    }

    return status;

} /* netlib_setsockopt */

/*
**++
**  ROUTINE:	netlib_getsockopt
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
unsigned int netlib_getsockopt (struct CTX **xctx,
    	    	    	    unsigned int *level, unsigned int *option,
    	    	    	    void *value, unsigned int *valsize,
    	    	    	    unsigned int *vallen,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm) {

    struct CTX *ctx;
    unsigned int status, lev;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (argc < 5) return SS$_INSFARG;
    if (option == 0 || value == 0 || valsize == 0) return SS$_BADPARAM;
    if (level == 0) lev = NETLIB_K_LEVEL_SOCKET;
    else lev = *level;

    if (argc > 7 && astadr != 0) {
    	struct IOR *ior;
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 8) ? astprm : 0);
    	ior->spec_retlen = vallen;
    	ior->spec_length = *valsize;
    	ior->iorflags = IOR_M_COPY_LENGTH;
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO$_GETSOCKOPT, &ior->iosb,
    	    	    	    io_completion, ior, lev, *option,
    	    	    	    value, &ior->spec_length, 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	unsigned int length;
    	length = *valsize;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO$_GETSOCKOPT,
    	    	    	    &myiosb, 0, 0, lev, *option, value,
    	    	    	    &length, 0, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 6 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    	if (OK(status) && argc > 5 && vallen != 0) *vallen = length;
    }

    return status;

} /* netlib_getsockopt */

/*
**++
**  ROUTINE:	netlib_name_to_address
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
unsigned int netlib_name_to_address (struct CTX **xctx, unsigned int *whichp,
    	    	    	    struct dsc$descriptor *namdsc,
    	    	    	    struct INADDRDEF *addrlist,
    	    	    	    unsigned int *listsize, unsigned int *count,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm) {

    struct CTX *ctx;
    struct INADDRDEF addr;
    unsigned int status, which;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (argc < 5) return SS$_INSFARG;

    if (namdsc == 0 || addrlist == 0 || listsize == 0) return SS$_BADPARAM;

    if (OK(netlib_strtoaddr(namdsc, &addr))) {
    	addrlist[0] = addr;
    	if (argc > 5 && count != 0) *count = 1;
    	if (argc > 6 && iosb != 0) {
    	    iosb->iosb_w_status = SS$_NORMAL;
    	    iosb->iosb_w_count = 1;
    	    iosb->iosb_l_unused = 0;
    	}
    	if (argc > 7 && astadr != 0) {
    	    return sys$dclast(astadr, (argc > 8) ? astprm : 0, 0);
    	} else {
    	    return SS$_NORMAL;
    	}
    }

    if (whichp == 0) which = NETLIB_K_LOOKUP_DNS;
    else which = *whichp;

    if (which == NETLIB_K_LOOKUP_DNS) {
    	return netlib___dns_name_to_addr(ctx, namdsc, addrlist, *listsize,
    	    	(argc > 5) ? count : 0,
    	    	(argc > 6) ? iosb  : 0,
    	    	(argc > 7) ? astadr : 0,
    	    	(argc > 8) ? astprm : 0);
    }

    return netlib___ht_name_to_addr(ctx, namdsc, addrlist, *listsize,
    	    	(argc > 5) ? count : 0,
    	    	(argc > 6) ? iosb  : 0,
    	    	(argc > 7) ? astadr : 0,
    	    	(argc > 8) ? astprm : 0);

} /* netlib_name_to_address */

/*
**++
**  ROUTINE:	netlib_address_to_name
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
unsigned int netlib_address_to_name (struct CTX **xctx, unsigned int *whichp,
    	    	    	    struct INADDRDEF *addr,
    	    	    	    unsigned int *addrsize,
    	    	    	    struct dsc$descriptor *namdsc,
    	    	    	    unsigned short *retlen,
    	    	    	    struct NETLIBIOSBDEF *iosb,
    	    	    	    void (*astadr)(), void *astprm) {

    struct CTX *ctx;
    unsigned int status, which;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (argc < 5) return SS$_INSFARG;

    if (addr == 0 || addrsize == 0 || namdsc == 0) return SS$_BADPARAM;

    if (whichp == 0) which = NETLIB_K_LOOKUP_DNS;
    else which = *whichp;

    if (which == NETLIB_K_LOOKUP_DNS) {
    	return netlib___dns_addr_to_name(ctx, addr, *addrsize, namdsc,
    	    	(argc > 5) ? retlen : 0,
    	    	(argc > 6) ? iosb   : 0,
    	    	(argc > 7) ? astadr : 0,
    	    	(argc > 8) ? astprm : 0);
    }

    return netlib___ht_addr_to_name(ctx, addr, *addrsize, namdsc,
    	    	(argc > 5) ? retlen : 0,
    	    	(argc > 6) ? iosb   : 0,
    	    	(argc > 7) ? astadr : 0,
    	    	(argc > 8) ? astprm : 0);

} /* netlib_address_to_name */

/*
**++
**  ROUTINE:	netlib___cvt_status
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
static unsigned int netlib___cvt_status (struct NETLIBIOSBDEF *iosb) {

    int errnum;

    if (iosb->iosb_w_status == SS$_NORMAL) return SS$_NORMAL;
    else if (iosb->iosb_w_status == SS$_CANCEL) return SS$_TIMEOUT;

    errnum = (iosb->iosb_w_status & 0x7fff) >> 3;
    if (errnum > sizeof(errorvec)/sizeof(errorvec[0])) return SS$_ABORT;
    return errorvec[errnum-1];

} /* netlib___cvt_status */

/*
**++
**  ROUTINE:	netlib___cvt_iosb
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
static void netlib___cvt_iosb (struct NETLIBIOSBDEF *dst,
    	    	    	    	struct NETLIBIOSBDEF *src) {

    int errnum;

    switch (src->iosb_w_status) {

    case SS$_NORMAL:
    case SS$_LINKDISCON:
    case SS$_TIMEOUT:
    	dst->iosb_w_status = src->iosb_w_status;
    	break;
    default:
    	errnum = (src->iosb_w_status & 0x7fff) >> 3;
    	dst->iosb_w_status = 
    	    (errnum > sizeof(errorvec)/sizeof(errorvec[0])) ? SS$_ABORT
    	    	    	    	    	    	    	  : errorvec[errnum-1];
    }
    dst->iosb_w_count = src->iosb_w_count;
    dst->iosb_l_unused = 0;

} /* netlib___cvt_iosb */

/*
**++
**  ROUTINE:	netlib_dns_mx_lookup
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
unsigned int netlib_dns_mx_lookup(struct CTX **xctx,
    	    	    struct dsc$descriptor *namdsc, struct MXRRDEF *mxrr,
    	    	    unsigned int *mxrrsize, unsigned int *mxrrcount,
    	    	    struct NETLIBIOSBDEF *iosb, void (*astadr)(), void *astprm) {

    struct CTX *ctx;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);
    if (argc < 4) return SS$_INSFARG;

    return netlib___dns_mx_lookup(xctx, namdsc, mxrr, mxrrsize,
    	    (argc > 4) ? mxrrcount : 0, (argc > 5) ? iosb : 0,
    	    (argc > 6) ? astadr : 0, (argc > 7) ? astprm : 0);

} /* netlib_dns_mx_lookup */
