/*
**++
**  FACILITY:	NETLIB
**
**  ABSTRACT:	Routines specific to CMU TCP/IP.
**
**  MODULE DESCRIPTION:
**
**  	tbs
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1994, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  26-OCT-1994
**
**  MODIFICATION HISTORY:
**
**  	31-OCT-1994 V1.0    Madison 	Initial coding.
**  	19-NOV-1994 V1.1    Madison 	Add CMU resolver bypass.
**  	09-JAN-1995 V1.1-1  Madison 	Fix $CANTIM coding error.
**  	13-JAN-1995 V1.1-2  Madison 	Fix potential memory leaks, timer bug.
**  	23-JAN-1995 V1.1-3  Madison 	Fix ASTPRM check on async close.
**  	17-FEB-1995 V1.1-4  Madison 	Fix UDP connects.
**  	28-FEB-1995 V1.1-5  Madison 	UDP sockets need to be OPENed;
**  	    	    	    	    	    arg missing on $DCLAST.
**  	08-JUN-1995 V1.1-6  Madison 	COPY_FROM was getting wiped out by TIMED.
**  	13-OCT-1995 V1.2    Madison 	Lots more fixes.
**--
*/
#include "netlib_cmu.h"
#include "netlib.h"
#include <lnmdef.h>
/*
**  Forward declarations
*/
    unsigned int netlib_socket(struct CTX **xctx, unsigned int *type,
    	    	    	    	unsigned int *family);
    unsigned int netlib_server_setup(struct CTX **xctx, struct SINDEF *sa,
    	    	    	    	unsigned int *salen);
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
    unsigned int netlib_get_hostname(void *bufdsc, unsigned short *retlenp);
    int netlib___get_nameservers(QUEUE *nsq);
    int netlib___get_domain(char *buf, unsigned short bufsize, unsigned short *retlen);
    static unsigned int netlib___cvt_status(struct NETLIBIOSBDEF *iosb);
    static void netlib___cvt_iosb(struct NETLIBIOSBDEF *dst, struct NETLIBIOSBDEF *src);
    static void expand(char *, unsigned int, unsigned char *, unsigned int *);
    unsigned int netlib_dns_mx_lookup(struct CTX **xctx,
    	    	    struct dsc$descriptor *namdsc, struct MXRRDEF *mxrr,
    	    	    unsigned int *mxrrsize, unsigned int *mxrrcount,
    	    	    struct NETLIBIOSBDEF *iosb, void (*astadr)(), void *astprm);
    static int bypass_cmu_resolver(void);
/*
**  OWN storage
*/
    static $DESCRIPTOR(inetdevice, "INET$DEVICE");
    static $DESCRIPTOR(backup_device, "_IP0:");
    static unsigned int rr_info_size = sizeof(struct CMU_RR);
    static unsigned int name_info_size = sizeof(struct CMU_AtoN);
    static unsigned int addr_info_size = sizeof(struct CMU_NtoA);
    static unsigned int conn_info_size = sizeof(struct CMU_ConnInfo);

/*
**  External references
*/
    unsigned int netlib_strtoaddr(struct dsc$descriptor *, struct INADDRDEF *);
    unsigned int netlib___dns_name_to_addr(struct CTX *ctx,
    	    	    struct dsc$descriptor *namdsc, struct INADDRDEF *addrlist,
    	    	    unsigned int listsize, unsigned int *count,
    	    	    struct NETLIBIOSBDEF *iosb, void (*astadr)(), void *astprm);
    unsigned int netlib___dns_addr_to_name(struct CTX *ctx,
    	    	    struct INADDRDEF *addrlist, unsigned int addrsize,
    	    	    struct dsc$descriptor *namdsc, unsigned short *retlen,
    	    	    struct NETLIBIOSBDEF *iosb, void (*astadr)(), void *astprm);
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
    struct {
    	unsigned short protocol;
    	unsigned char  type;
    	unsigned char  domain;
    } sockdef;
    unsigned int status, af, ty;
    int argc;

    SETARGCOUNT(argc);

    if (argc < 1) return SS$_INSFARG;

    status = netlib___alloc_ctx(&ctx, SPECCTX_SIZE);
    if (!OK(status)) return status;

    status = sys$assign(&inetdevice, &ctx->chan, 0, 0);
    if (!OK(status)) status = sys$assign(&backup_device, &ctx->chan, 0, 0);
    if (!OK(status)) {
    	netlib___free_ctx(ctx);
    	return status;
    }

    ctx->specctx->socket_type =
    	(argc < 2 || type == 0) ? NETLIB_K_TYPE_STREAM : *type;

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
    struct NETLIBIOSBDEF iosb;
    unsigned int status, type, af;

    if (sa == 0 || salen == 0) return SS$_BADPARAM;

    type = NETLIB_K_TYPE_STREAM;
    af = NETLIB_K_AF_INET;
    status = netlib_socket(&ctx, &type, &af);

    status = sys$qiow(netlib_synch_efn, ctx->chan, IO__OPEN,
    	    	    	&iosb, 0, 0,
    	    	    	0, 0, netlib_word_swap(sa->sin_w_port),
    	    	    	0, CMU_K_PROTO_TCP, 0);
    if (OK(status)) status = netlib___cvt_status(&iosb);
    if (!OK(status)) {
    	sys$dassgn(ctx->chan);
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
    unsigned int status;
    int argc;
    static unsigned int one = 1;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (sa == 0 || salen == 0) return SS$_BADPARAM;
    if (*salen < sizeof(struct SINDEF)) return SS$_BADPARAM;

    ctx->specctx->local_addr = sa->sin_x_addr;
    ctx->specctx->local_port = netlib_word_swap(sa->sin_w_port);
    if (ctx->specctx->socket_type == NETLIB_K_TYPE_STREAM) {
    	if (argc > 3 && iosb != 0) {
    	    iosb->iosb_w_status = SS$_NORMAL;
    	    iosb->iosb_w_count = 0;
    	    iosb->iosb_l_unused = 0;
    	}
    	if (argc > 4 && astadr != 0) {
    	    sys$dclast(astadr, (argc > 5) ? astprm : 0, 0);
    	}
    	return SS$_NORMAL;
    }

    if (argc > 4 && astadr != 0) {
    	struct IOR *ior;
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 5) ? astprm : 0);
    	ior->iorflags |= IOR_M_SET_OPENFLAG;
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO__OPEN, &ior->iosb,
    	    	    	    io_completion, ior, 0, 0, ctx->specctx->local_port,
    	    	    	    CMU_M_OPEN_UDPDATA, CMU_K_PROTO_UDP, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO__OPEN, &myiosb,
    	    	    	    0, 0, 0, 0, ctx->specctx->local_port,
    	    	    	    CMU_M_OPEN_UDPDATA, CMU_K_PROTO_UDP, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 3 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    	if (OK(status)) ctx->specctx->flags |= SPECCTX_M_OPEN;
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

    ior->iorflags |= IOR_M_IO_COMPLETED;

    if (ior->iorflags & IOR_M_IO_TIMED) sys$cantim(ior, 0);

    if (ior->iosb.iosb_w_status == SS$_CANCEL)
    	ior->iosb.iosb_w_status = SS$_TIMEOUT;

    if (ior->iosbp != 0) netlib___cvt_iosb(ior->iosbp, &ior->iosb);

    if (ior->iorflags & IOR_M_COPY_FROM) {
    	if (OK(ior->iosb.iosb_w_status) && ior->spec_userfrom != 0) {
    	    unsigned int len;
    	    struct SINDEF *sa;

    	    sa = ior->spec_userfrom;
    	    len = sizeof(struct SINDEF);
    	    if (len > ior->spec_length) len = ior->spec_length;
    	    if (len >= 2) sa->sin_w_family = NETLIB_K_AF_INET;
    	    if (len >= 4) sa->sin_w_port =
    	        netlib_word_swap(ior->specior.from.ipa_w_srcport);
    	    if (len >= 8) sa->sin_x_addr = ior->specior.from.ipa_x_srcaddr;
    	    if (len > 8) memset(sa->sin_x_mbz, 0, len-8);
    	    if (ior->spec_retlen != 0) *(unsigned int *) ior->spec_retlen = len;
    	}
    	ior->iorflags &= ~IOR_M_COPY_FROM;
    }

    if (ior->iorflags & IOR_M_COPY_CI_LOCAL) {
    	if (OK(ior->iosb.iosb_w_status)) {
    	    struct CMU_ConnInfo *ci;
    	    struct SINDEF *sa;
    	    int len;

    	    ci = ior->spec_conn_info;
    	    sa = ior->spec_userfrom;
    	    len = sizeof(struct SINDEF);
    	    if (ior->spec_usersize < len) len = ior->spec_usersize;
    	    if (len >= 2) sa->sin_w_family = NETLIB_K_AF_INET;
    	    if (len >= 4) sa->sin_w_port = netlib_word_swap(ci->ci_l_local_port);
    	    if (len >= 8) sa->sin_x_addr = ci->ci_x_local_address;
    	    if (len > 8) memset(sa->sin_x_mbz, 0, len-8);
    	    if (ior->spec_retlen != 0) *(unsigned int *)ior->spec_retlen = len;
    	}
    	lib$free_vm(&conn_info_size, &ior->spec_conn_info);
    	ior->iorflags &= ~IOR_M_COPY_CI_LOCAL;
    }

    if (ior->iorflags & IOR_M_COPY_CI_REMOTE) {
    	if (OK(ior->iosb.iosb_w_status)) {
    	    struct CMU_ConnInfo *ci;
    	    struct SINDEF *sa;
    	    int len;
    	    ci = ior->spec_conn_info;
   	    sa = ior->spec_userfrom;
    	    len = sizeof(struct SINDEF);
    	    if (ior->spec_usersize < len) len = ior->spec_usersize;
    	    if (len >= 2) sa->sin_w_family = NETLIB_K_AF_INET;
    	    if (len >= 4) sa->sin_w_port = netlib_word_swap(ci->ci_l_remote_port);
    	    if (len >= 8) sa->sin_x_addr = ci->ci_x_remote_address;
    	    if (len > 8) memset(sa->sin_x_mbz, 0, len-8);
    	    if (ior->spec_retlen != 0) *(unsigned int *)ior->spec_retlen = len;
    	}
    	lib$free_vm(&conn_info_size, &ior->spec_conn_info);
    	ior->iorflags &= ~IOR_M_COPY_CI_REMOTE;
    }

    if (ior->iorflags & IOR_M_COPY_ADDRS) {
    	if (OK(ior->iosb.iosb_w_status)) {
    	    struct INADDRDEF *alist;
    	    struct CMU_NtoA *ntoa = ior->spec_addr_info;
    	    int i;

    	    alist = ior->spec_userbuf;
    	    for (i = 0; i < ior->spec_length && i < ntoa->ntoa_l_count; i++) {
    	    	alist[i] = ntoa->ntoa_x_addr[i];
    	    }
    	    if (ior->spec_retlen != 0) *(unsigned int *)ior->spec_retlen = i;
    	}
    	lib$free_vm(&addr_info_size, &ior->spec_addr_info);
    	ior->iorflags &= ~IOR_M_COPY_ADDRS;
    }

    if (ior->iorflags & IOR_M_COPY_HOSTNAME) {
        if (OK(ior->iosb.iosb_w_status)) {
    	    struct dsc$descriptor *dsc;
    	    struct CMU_AtoN *ni = ior->spec_name_info;
    	    dsc = ior->spec_userbuf;
    	    str$copy_r(dsc, &ni->aton_l_namelen, ni->aton_t_name);
    	    if (ior->spec_retlen != 0) {
    	        *(unsigned short *) ior->spec_retlen = ni->aton_l_namelen;
    	    }
    	}
    	lib$free_vm(&name_info_size, &ior->spec_name_info);
    	ior->iorflags &= ~IOR_M_COPY_HOSTNAME;
    }

    if (ior->iorflags & IOR_M_COPY_MXRRS) {
    	if (OK(ior->iosb.iosb_w_status)) {
    	    struct MXRRDEF *mxrr;
    	    struct CMU_RR *rr = ior->spec_rr_info;
    	    unsigned int i, len;
    	    int remain;
    	    unsigned char *cp;

    	    mxrr = ior->spec_userbuf;
    	    cp = rr->rr_x_data + 1;
    	    remain = rr->rr_w_length - 1;
    	    i = 0;
    	    while (remain > 0 && i < ior->spec_length) {
    	        len = *(unsigned short *)cp; cp += 2;
    	        mxrr[i].mxrr_l_preference = *(unsigned short *) cp;
    	    	expand(mxrr[i].mxrr_t_name, NETLIB_S_MXRR_NAME, cp + 2,
    	    	    	&mxrr[i].mxrr_l_length);
    	        remain -= len + 2;
    	        cp += len;
    	        i++;
    	    }
    	    if (ior->spec_retlen != 0) *(unsigned int *)ior->spec_retlen = i;
    	}
    	lib$free_vm(&rr_info_size, &ior->spec_rr_info);
    	ior->iorflags &= ~IOR_M_COPY_MXRRS;
    }

    if (ior->iorflags & IOR_M_NEW_CONTEXT) {
    	if (OK(ior->iosb.iosb_w_status)) {
    	    *(struct CTX **) ior->spec_xnewctx = ior->spec_newctx;
    	    ior->iorflags &= ~IOR_M_NEW_CONTEXT;
    	    if (ior->spec_userfrom != 0) {
    	        unsigned int len;
    	        len = ior->spec_length;
    	        ior->iosb.iosb_w_status =
    	    	    netlib_getpeername(&(struct CTX *) ior->spec_newctx,
    	       	    	ior->spec_userfrom, &len,
    	    	    	ior->spec_retlen, ior->iosbp,
    	    	    	ior->astadr, ior->astprm);
    	    	if (OK(ior->iosb.iosb_w_status)) {
    	    	    FREE_IOR(ior);
    	    	    return SS$_NORMAL;
    	    	} else {
    	    	    if (ior->iosbp != 0) netlib___cvt_iosb(ior->iosbp, &ior->iosb);
    	    	    sys$dassgn(((struct CTX *)ior->spec_newctx)->chan);
    	    	    netlib___free_ctx((struct CTX *) ior->spec_newctx);
    	    	}
    	    }
    	} else {
    	    sys$dassgn(((struct CTX *)ior->spec_newctx)->chan);
    	    netlib___free_ctx((struct CTX *) ior->spec_newctx);
    	}
    	ior->iorflags &= ~IOR_M_NEW_CONTEXT;
    }

    if (ior->iorflags & IOR_M_SET_OPENFLAG) {
    	if (OK(ior->iosb.iosb_w_status))
    	    ior->ctx->specctx->flags |= SPECCTX_M_OPEN;
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
    ITMLST slst;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (sa == 0 || sasize == 0) return SS$_BADPARAM;

    if (argc > 5 && astadr != 0) {
    	struct IOR *ior;

    	GET_IOR(ior, ctx, iosb, astadr, (argc > 6) ? astprm : 0);
    	status = lib$get_vm(&conn_info_size, &ior->spec_conn_info);
    	if (!OK(status)) {
    	    FREE_IOR(ior);
    	    return status;
    	}
    	ior->spec_usersize = *sasize;
    	ior->spec_userfrom = sa;
    	ior->spec_retlen = salen;
    	ior->iorflags = IOR_M_COPY_CI_LOCAL;
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO__INFO, &ior->iosb,
    	    	    	    io_completion, ior, ior->spec_conn_info, 
    	    	    	    conn_info_size, 0, 0, 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	struct CMU_ConnInfo ci;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO__INFO,
    	    	    	    &myiosb, 0, 0, &ci,
    	    	    	    conn_info_size, 0, 0, 0, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 4 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    	if (OK(status)) {
    	    int len;
    	    len = sizeof(struct SINDEF);
    	    if (*sasize < len) len = *sasize;
    	    if (len >= 2) sa->sin_w_family = NETLIB_K_AF_INET;
    	    if (len >= 4) sa->sin_w_port = netlib_word_swap(ci.ci_l_local_port);
    	    if (len >= 8) sa->sin_x_addr = ci.ci_x_local_address;
    	    if (len > 8) memset(sa->sin_x_mbz, 0, len-8);
    	    if (argc > 3 && salen != 0) *salen = len;
    	}
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
    ITMLST slst;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (sa == 0 || sasize == 0) return SS$_BADPARAM;

    if (argc > 5 && astadr != 0) {
    	struct IOR *ior;

    	GET_IOR(ior, ctx, iosb, astadr, (argc > 6) ? astprm : 0);
    	status = lib$get_vm(&conn_info_size, &ior->spec_conn_info);
    	if (!OK(status)) {
    	    FREE_IOR(ior);
    	    return status;
    	}
    	ior->spec_userfrom = sa;
    	ior->spec_usersize = *sasize;
    	ior->spec_retlen = salen;
    	ior->iorflags = IOR_M_COPY_CI_REMOTE;
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO__INFO, &ior->iosb,
    	    	    	    io_completion, ior, ior->spec_conn_info, 
    	    	    	    conn_info_size, 0, 0, 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	struct CMU_ConnInfo ci;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO__INFO,
    	    	    	    &myiosb, 0, 0, &ci,
    	    	    	    conn_info_size, 0, 0, 0, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 4 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    	if (OK(status)) {
    	    int len;
    	    len = sizeof(struct SINDEF);
    	    if (*sasize < len) len = *sasize;
    	    if (len >= 2) sa->sin_w_family = NETLIB_K_AF_INET;
    	    if (len >= 4) sa->sin_w_port = netlib_word_swap(ci.ci_l_remote_port);
    	    if (len >= 8) sa->sin_x_addr = ci.ci_x_remote_address;
    	    if (len > 8) memset(sa->sin_x_mbz, 0, len-8);
    	    if (argc > 3 && salen != 0) *salen = len;
    	}
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

/*
**  For UDP sockets, save the remote address/port for later use
*/
    if (ctx->specctx->socket_type == NETLIB_K_TYPE_DGRAM) {

    	ctx->specctx->remote_addr = sa->sin_x_addr;
    	ctx->specctx->remote_port = sa->sin_w_port;

    	if (argc > 4 && astadr != 0) {
    	    struct IOR *ior;
    	    GET_IOR(ior, ctx, iosb, astadr, (argc > 5) ? astprm : 0);
    	    status = sys$qio(netlib_asynch_efn, ctx->chan, IO__OPEN, &ior->iosb,
    	    	    	    io_completion, ior, 0, 0, 0,
    	    	    	    CMU_M_OPEN_UDPDATA, CMU_K_PROTO_UDP, 0);
    	    if (!OK(status)) FREE_IOR(ior);
    	} else {
    	    struct NETLIBIOSBDEF myiosb;
    	    status = sys$qiow(netlib_synch_efn, ctx->chan, IO__OPEN, &myiosb,
    	    	    	    0, 0, 0, 0, 0,
    	    	    	    CMU_M_OPEN_UDPDATA, CMU_K_PROTO_UDP, 0);
    	    if (OK(status)) status = netlib___cvt_status(&myiosb);
    	    if (argc > 3 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    	}

    	return status;

    }

    if (argc > 4 && astadr != 0) {
    	struct IOR *ior;

    	GET_IOR(ior, ctx, iosb, astadr, (argc > 5) ? astprm : 0);
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO__OPEN, &ior->iosb,
    	    	    	    io_completion, ior,
    	    	    	    &sa->sin_x_addr.inaddr_l_addr,
    	    	    	    netlib_word_swap(sa->sin_w_port),
    	    	    	    ctx->specctx->local_port,
    	    	    	    CMU_M_OPEN_ACTIVE|CMU_M_OPEN_BYADDRESS,
    	    	    	    CMU_K_PROTO_TCP, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO__OPEN, &myiosb, 0, 0,
    	    	    	    &sa->sin_x_addr.inaddr_l_addr,
    	    	    	    netlib_word_swap(sa->sin_w_port),
    	    	    	    ctx->specctx->local_port,
    	    	    	    CMU_M_OPEN_ACTIVE|CMU_M_OPEN_BYADDRESS,
    	    	    	    CMU_K_PROTO_TCP, 0);
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
    struct CMU_IPAddr ipa;
    int argc, do_address;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    status = lib$analyze_sdesc(dsc, &buflen, &bufptr);
    if (!OK(status)) return status;

    if ((ctx->specctx->socket_type == NETLIB_K_TYPE_DGRAM) &&
    	    !(ctx->specctx->flags & SPECCTX_M_OPEN)) {
    	struct NETLIBIOSBDEF myiosb;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO__OPEN, &myiosb,
    	    	    	    0, 0, 0, 0, ctx->specctx->local_port,
    	    	    	    CMU_M_OPEN_UDPDATA, CMU_K_PROTO_UDP, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (!OK(status)) return status;
    	ctx->specctx->flags |= SPECCTX_M_OPEN;
    }

    do_address = 0;
    if (argc > 3 && salen != 0) {
    	do_address = 1;
    	memset(&ipa, 0, sizeof(ipa));
    	if (*salen >= 4) ipa.ipa_w_dstport = netlib_word_swap(sa->sin_w_port);
    	if (*salen >= 8) ipa.ipa_x_dstaddr = sa->sin_x_addr;
    	ipa.ipa_w_srcport = ctx->specctx->local_port;
    	ipa.ipa_x_srcaddr = ctx->specctx->local_addr;
    } else if (ctx->specctx->socket_type == NETLIB_K_TYPE_DGRAM) {
    	do_address = 1;
    	memset(&ipa, 0, sizeof(ipa));
    	ipa.ipa_w_dstport = netlib_word_swap(ctx->specctx->remote_port);
    	ipa.ipa_x_dstaddr = ctx->specctx->remote_addr;
    	ipa.ipa_w_srcport = ctx->specctx->local_port;
    	ipa.ipa_x_srcaddr = ctx->specctx->local_addr;
    }

    if (argc > 5 && astadr != 0) {
    	struct IOR *ior;

    	GET_IOR(ior, ctx, iosb, astadr, (argc > 6) ? astprm : 0);
    	if (buflen == 0) {
    	    ior->iosb.iosb_w_status = SS$_NORMAL;
    	    ior->iosb.iosb_w_count = 0;
    	    ior->iosb.iosb_l_unused = 0;
    	    status = sys$dclast(io_completion, ior, 0);
    	} else {
    	    status = sys$qio(netlib_asynch_efn, ctx->chan, IO$_WRITEVBLK, &ior->iosb,
    	    	    	    io_completion, ior, bufptr, buflen, 0, 1, 0,
    	    	    	    do_address ? &ipa : 0);
    	    if (!OK(status)) FREE_IOR(ior);
    	}
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	if (buflen == 0) {
    	    status = myiosb.iosb_w_status = SS$_NORMAL;
    	    myiosb.iosb_w_count = 0;
    	    myiosb.iosb_l_unused = 0;
    	} else {
    	    status = sys$qiow(netlib_synch_efn, ctx->chan, IO$_WRITEVBLK,
    	    	    	    &myiosb, 0, 0, bufptr, buflen, 0, 1, 0,
    	    	    	    do_address ? &ipa : 0);
    	    if (OK(status)) status = netlib___cvt_status(&myiosb);
    	}
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
    struct IOR *ior;
    unsigned int status;
    int argc, do_from;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (dsc->dsc$b_dtype != DSC$K_DTYPE_T && dsc->dsc$b_dtype != 0) {
    	return SS$_BADPARAM;
    }

    if (dsc->dsc$b_class != DSC$K_CLASS_S && dsc->dsc$b_class != 0) {
    	return SS$_BADPARAM;
    }

    if ((ctx->specctx->socket_type == NETLIB_K_TYPE_DGRAM) &&
    	    !(ctx->specctx->flags & SPECCTX_M_OPEN)) {
    	struct NETLIBIOSBDEF myiosb;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO__OPEN, &myiosb,
    	    	    	    0, 0, 0, 0, ctx->specctx->local_port,
    	    	    	    CMU_M_OPEN_UDPDATA, CMU_K_PROTO_UDP, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (!OK(status)) return status;
    	ctx->specctx->flags |= SPECCTX_M_OPEN;
    }

    do_from = (argc > 3 && sa != 0 && sasize != 0 && *sasize != 0);
    if (argc > 7 && astadr != 0) {
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 8) ? astprm : 0);
    	if (do_from) {
    	    ior->spec_userfrom = sa;
    	    ior->spec_length = *sasize;
    	    ior->spec_retlen = salen;
    	    ior->iorflags = IOR_M_COPY_FROM;
    	} else ior->iorflags = 0;
    	ior->iorflags &= ~IOR_M_IO_COMPLETED;
    	if (timeout != 0) {    	    
    	    ior->iorflags |= IOR_M_IO_TIMED;
    	    status = sys$setimr(netlib_asynch_efn, timeout, io_timeout, ior);
    	    if (!OK(status)) {
    	    	FREE_IOR(ior);
    	    	return status;
    	    }
    	}
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO$_READVBLK, &ior->iosb,
    	    	    	    io_completion, ior, dsc->dsc$a_pointer,
    	    	    	    dsc->dsc$w_length,
    	    	    	    do_from ? &ior->specior.from : 0, 0, 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	struct CMU_IPAddr from;

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
    	    	    	    dsc->dsc$a_pointer, dsc->dsc$w_length,
    	    	    	    do_from ? &from : 0, 0, 0, 0);
    	if (argc > 5 && timeout != 0) {
    	    sys$cantim(ior, 0);
    	    FREE_IOR(ior);
    	}
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 6 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    	if (OK(status) && do_from) {
    	    unsigned int len;
    	    len = sizeof(struct SINDEF);
    	    if (len > *sasize) len = *sasize;
    	    if (len >= 2) sa->sin_w_family = NETLIB_K_AF_INET;
    	    if (len >= 4) sa->sin_w_port =
    	    	netlib_word_swap(from.ipa_w_srcport);
    	    if (len >= 8) sa->sin_x_addr = from.ipa_x_srcaddr;
    	    if (len > 8) memset(sa->sin_x_mbz, 0, len-8);
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

    	GET_IOR(ior, ctx, iosb, astadr, (argc > 4) ? astprm : 0);
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO__CLOSE, &ior->iosb,
    	    	    	    io_completion, ior, 0, 0, 0, 0, 0, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	status = sys$qiow(netlib_synch_efn, ctx->chan, IO__CLOSE, &myiosb,
    	    	    	    0, 0, 0, 0, 0, 0, 0, 0);
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
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (argc > 2 && iosb != 0) {
    	iosb->iosb_w_status = SS$_NORMAL;
    	iosb->iosb_w_count = 0;
    	iosb->iosb_l_unused = 0;
    }

    if (argc > 3 && astadr != 0) sys$dclast(astadr, argc > 4 ? astprm : 0, 0);

    return SS$_NORMAL;

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
    ITMLST sname;
    unsigned int status;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (argc < 2) return SS$_INSFARG;
    if (xnewctx == 0) return SS$_BADPARAM;
    status = netlib___alloc_ctx(&newctx, SPECCTX_SIZE);
    if (!OK(status)) return status;

    status = sys$assign(&inetdevice, &newctx->chan, 0, 0);
    if (!OK(status)) status = sys$assign(&backup_device, &newctx->chan, 0, 0);
    if (!OK(status)) {
    	netlib___free_ctx(newctx);
    	return status;
    }

    if (argc > 6 && astadr != 0) {
    	struct IOR *ior;

    	GET_IOR(ior, ctx, iosb, astadr, (argc > 7) ? astprm : 0);
    	ior->spec_userfrom = (sasize == 0) ? 0 : sa;
    	ior->spec_length = (sasize == 0) ? 0 : *sasize;
    	ior->spec_retlen = salen;
    	ior->spec_xnewctx = xnewctx;
    	ior->spec_newctx = newctx;
    	ior->iorflags = IOR_M_NEW_CONTEXT;
    	status = sys$qio(netlib_asynch_efn, newctx->chan, IO__OPEN,
    	    	    	    &ior->iosb, io_completion, ior,
    	    	    	    0, 0, ctx->specctx->local_port, 0,
    	    	    	    CMU_K_PROTO_TCP, 0);
    	if (!OK(status)) FREE_IOR(ior);
    } else {
    	struct NETLIBIOSBDEF myiosb;
    	status = sys$qiow(netlib_synch_efn, newctx->chan, IO__OPEN,
    	    	    	    &myiosb, 0, 0, 0, 0,
    	    	    	    ctx->specctx->local_port, 0, CMU_K_PROTO_TCP, 0);
    	if (OK(status)) status = netlib___cvt_status(&myiosb);
    	if (argc > 5 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    	if (OK(status)) {
    	    *xnewctx = newctx;
    	    if (argc > 3 && sa != 0 && sasize != 0) {
    	    	status = netlib_getpeername(&newctx, sa, sasize,
    	    	    	    	    (argc > 4) ? salen : 0, 0, 0, 0);
    	    	if (!OK(status)) sys$dassgn(newctx->chan);
    	    }
    	}
    }

    if (!OK(status)) netlib___free_ctx(ctx);

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

    if (argc > 5 && iosb != 0) {
    	iosb->iosb_w_status = SS$_NORMAL;
    	iosb->iosb_w_count = 0;
    	iosb->iosb_l_unused = 0;
    }

    if (argc > 6 && astadr != 0) sys$dclast(astadr, argc > 7 ? astprm : 0, 0);

    return SS$_NORMAL;

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

    if (argc > 5 && vallen != 0) *vallen = 0;

    if (argc > 6 && iosb != 0) {
    	iosb->iosb_w_status = SS$_NORMAL;
    	iosb->iosb_w_count = 0;
    	iosb->iosb_l_unused = 0;
    }
    if (argc > 7 && astadr != 0) sys$dclast(astadr, argc > 8 ? astprm : 0, 0);

    return SS$_NORMAL;

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
    struct CMU_NtoA  ntoa;
    struct INADDRDEF addr;
    struct NETLIBIOSBDEF myiosb;
    unsigned int status;
    unsigned short namlen;
    char *namp, nambuf[256];
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

    if (bypass_cmu_resolver()) {
    	return netlib___dns_name_to_addr(ctx, namdsc, addrlist, *listsize,
    	    	(argc > 5) ? count : 0,
    	    	(argc > 6) ? iosb  : 0,
    	    	(argc > 7) ? astadr : 0,
    	    	(argc > 8) ? astprm : 0);
    }

    status = lib$analyze_sdesc(namdsc, &namlen, &namp);
    if (!OK(status)) return status;

    if (namlen > sizeof(nambuf)-1) namlen = sizeof(nambuf)-1;
    memcpy(nambuf, namp, namlen);
    nambuf[namlen] = '\0';

    if (argc > 7 && astadr != 0) {
    	struct IOR *ior;

    	GET_IOR(ior, ctx, iosb, astadr, (argc > 8) ? astprm : 0);
    	status = lib$get_vm(&addr_info_size, &ior->spec_addr_info);
    	if (!OK(status)) {
    	    FREE_IOR(ior);
    	    return status;
    	}
    	ior->spec_userbuf = addrlist;
    	ior->spec_length = *listsize;
    	ior->spec_retlen = count;
    	ior->iorflags = IOR_M_COPY_ADDRS;
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO__GTHST, &ior->iosb,
    	    	    	    io_completion, ior,
    	    	    	    &ior->spec_addr_info, addr_info_size,
    	    	    	    CMU_K_GTHST_NAMEADDR, nambuf, 0, 0);
    	if (!OK(status)) {
    	    lib$free_vm(&addr_info_size, &ior->spec_addr_info);
    	    FREE_IOR(ior);
    	}
    	return status;
    }
    status = sys$qiow(netlib_synch_efn, ctx->chan, IO__GTHST, &myiosb,
    	    	    	0, 0, &ntoa, sizeof(ntoa),
    	    	    	CMU_K_GTHST_NAMEADDR, nambuf, 0, 0);
    if (OK(status)) status = netlib___cvt_status(&myiosb);
    if (argc > 6 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    if (OK(status)) {
    	int i;
    	for (i = 0; i < *listsize && i < ntoa.ntoa_l_count; i++) {
    	    addrlist[i] = ntoa.ntoa_x_addr[i];
    	}
    	if (argc > 5 && count != 0) *count = i;
    }

    return status;
    
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
    struct NETLIBIOSBDEF myiosb;
    struct CMU_AtoN aton;
    unsigned int status;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (argc < 5) return SS$_INSFARG;

    if (bypass_cmu_resolver()) {
    	return netlib___dns_addr_to_name(ctx, addr, *addrsize, namdsc,
    	    	(argc > 5) ? retlen : 0,
    	    	(argc > 6) ? iosb   : 0,
    	    	(argc > 7) ? astadr : 0,
    	    	(argc > 8) ? astprm : 0);
    }

    if (addr == 0 || addrsize == 0 || namdsc == 0) return SS$_BADPARAM;
    if (*addrsize != sizeof(struct INADDRDEF)) return SS$_BADPARAM;

    if (argc > 7 && astadr != 0) {
    	struct IOR *ior;

    	GET_IOR(ior, ctx, iosb, astadr, (argc > 8) ? astprm : 0);
    	status = lib$get_vm(&name_info_size, &ior->spec_name_info);
    	if (!OK(status)) {
    	    FREE_IOR(ior);
    	    return status;
    	}
    	ior->spec_userbuf = namdsc;
    	ior->spec_retlen = (unsigned int *) retlen;
    	ior->iorflags = IOR_M_COPY_HOSTNAME;
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO__GTHST, &ior->iosb,
    	    	    	    io_completion, ior,
    	    	    	    ior->spec_name_info, name_info_size,
    	    	    	    CMU_K_GTHST_ADDRNAME, addr->inaddr_l_addr, 0, 0);
    	if (!OK(status)) {
    	    lib$free_vm(&name_info_size, &ior->spec_name_info);
    	    FREE_IOR(ior);
    	}
    	return status;
    }
    status = sys$qiow(netlib_synch_efn, ctx->chan, IO__GTHST, &myiosb,
    	    	    	0, 0, &aton, sizeof(aton),
    	    	    	CMU_K_GTHST_ADDRNAME, addr->inaddr_l_addr, 0, 0);
    if (OK(status)) status = netlib___cvt_status(&myiosb);
    if (argc > 6 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    if (OK(status)) {
    	str$copy_r(namdsc, &aton.aton_l_namelen, aton.aton_t_name);
    	if (argc > 5 && retlen != 0)
    	    *retlen = aton.aton_l_namelen;
    }

    return status;

} /* netlib_address_to_name */

/*
**++
**  ROUTINE:	netlib_get_hostname
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
unsigned int netlib_get_hostname (void *bufdsc, unsigned short *retlenp) {

    char buf[256];
    ITMLST lnmlst[2];
    unsigned int status;
    unsigned short retlen;
    int argc;
    static $DESCRIPTOR(tabnam, "LNM$SYSTEM");
    static $DESCRIPTOR(lognam, "INTERNET_HOST_NAME");

    SETARGCOUNT(argc);

    ITMLST_INIT(lnmlst[0], LNM$_STRING, sizeof(buf), buf, &retlen);
    ITMLST_INIT(lnmlst[1], 0, 0, 0, 0);
    status = sys$trnlnm(0, &tabnam, &lognam, 0, lnmlst);
    if (OK(status)) status = str$copy_r(bufdsc, &retlen, buf);
    if (!OK(status)) return status;
    if (argc > 1 && retlenp != 0) *retlenp = retlen;

    return SS$_NORMAL;

} /* netlib_get_hostname */

/*
**++
**  ROUTINE:	netlib___get_nameservers
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
int netlib___get_nameservers (QUEUE *nsq) {

    struct NAMESERVER *ns;
    struct INADDRDEF a;
    struct CTX *tmpctx;
    struct dsc$descriptor dsc, lognamdsc;
    ITMLST lnmlst[4];
    char name[256], lognam[256], *cp, *anchor;
    unsigned int status, size, maxidx;
    unsigned short namlen;
    int index, i, remain;
    static unsigned int one = 1;

    static $DESCRIPTOR(nameserver_tabnam, "LNM$FILE_DEV");

    tmpctx = 0;

    ITMLST_INIT(lnmlst[0], LNM$_INDEX, sizeof(index), &index, 0);
    ITMLST_INIT(lnmlst[1], LNM$_STRING, sizeof(name), name, &namlen);
    ITMLST_INIT(lnmlst[2], 0, 0, 0, 0);

    INIT_SDESC(lognamdsc, 0, lognam);

    for (index = 0; index <= 16; index++) {
    	lognamdsc.dsc$w_length = sprintf(lognam, "NETLIB_NAMESERVERS", index);
    	status = sys$trnlnm(0, &nameserver_tabnam, &lognamdsc, 0, lnmlst);
    	if (!OK(status) || namlen == 0) continue;
    	for (anchor = name, remain = namlen; remain > 0;
    	    	    	    	    	remain -= i+1, anchor = cp+1) {
    	    cp = memchr(anchor, ',', remain);
    	    if (cp == 0) i = remain;
    	    else i = cp - anchor;
    	    INIT_SDESC(dsc, i, anchor);
    	    if (!OK(netlib_strtoaddr(&dsc, &a))) {
    	    	if (tmpctx == 0) {
    	    	    if (!OK(netlib_socket(&tmpctx, 0, 0))) continue;
    	    	}
    	    	if (!OK(netlib_name_to_address(&tmpctx, 0, &dsc, &a, &one, 0,
    	    	    	    	0, 0, 0))) continue;
    	    }
    	    size = sizeof(struct NAMESERVER);
    	    status = lib$get_vm(&size, &ns);
    	    if (!OK(status)) break;
    	    ns->addr = a;
    	    queue_insert(ns, nsq->tail);
    	}
    }

    if (tmpctx != 0) netlib_close(&tmpctx);
    return index;

} /* netlib___get_nameservers */

/*
**++
**  ROUTINE:	netlib___get_domain
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
int netlib___get_domain (char *buf, unsigned short bufsize, unsigned short *retlen) {

    struct dsc$descriptor dsc;
    ITMLST lnmlst[2];
    char name[256], *cp;
    unsigned int status;
    unsigned short namlen;

    static $DESCRIPTOR(lognamdsc, "NETLIB_DOMAIN");
    static $DESCRIPTOR(tabnam, "LNM$FILE_DEV");

    ITMLST_INIT(lnmlst[0], LNM$_STRING, sizeof(name), name, &namlen);
    ITMLST_INIT(lnmlst[1], 0, 0, 0, 0);

    status = sys$trnlnm(0, &tabnam, &lognamdsc, 0, lnmlst);
    if (OK(status)) cp = name;
    else {
    	INIT_SDESC(dsc, sizeof(name), name);
    	if (!OK(netlib_get_hostname(&dsc, &namlen))) return 0;
    	cp = memchr(name, '.', namlen);
    	if (cp == 0) return 0;
    	cp += 1;
    	namlen -= (cp - name);
    }

    if (memchr(cp, '.', namlen) == 0) return 0;
    if (namlen > bufsize-1) namlen = bufsize-1;
    memcpy(buf, cp, namlen);
    *retlen = namlen;

    return 1;

} /* netlib___get_domain */

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

    struct CMUIosb *cmuiosb;

    if (iosb->iosb_w_status == SS$_CANCEL) iosb->iosb_w_status = SS$_TIMEOUT;
    if (iosb->iosb_w_status != SS$_ABORT) return iosb->iosb_w_status;

    cmuiosb = (struct CMUIosb *) iosb;
    switch (cmuiosb->cmu_status) {
    	case NET$_IR:
    	case NET$_UCT:
    	case NET$_CSE:
    	case NET$_VTF:
    	    return SS$_INSFMEM;
    	case NET$_IFC:
    	case NET$_IPC:
    	case NET$_IFS:
    	case NET$_ILP:
    	case NET$_NUC:
    	case NET$_CIP:
    	case NET$_CDE:
    	case NET$_FSU:
    	    return SS$_BADPARAM;
    	case NET$_NOPRV:
    	case NET$_NOINA:
    	case NET$_NOANA:
    	    return SS$_NOPRIV;
    	case NET$_CC:
    	case NET$_CR:
    	case NET$_CCAN:
    	case NET$_KILL:
    	    return SS$_LINKDISCON;
    	case NET$_UNN:
    	case NET$_UNA:
    	case NET$_UNU:
    	    return SS$_IVADDR;
    	case NET$_BTS:
    	    return SS$_IVBUFLEN;
    	case NET$_URC:
    	case NET$_NRT:
    	    return SS$_UNREACHABLE;
    	case NET$_CTO:
    	case NET$_TWT:
    	case NET$_FTO:
    	case NET$_NMLTO:
    	    return SS$_TIMEOUT;
    	case NET$_TE:
    	case NET$_NSEXIT:
    	    return SS$_SHUT;
    	case NET$_DSNOADDR:
    	case NET$_DSNONAME:
    	case NET$_DSNAMERR:
    	case NET$_DSREFEXC:
    	    return SS$_ENDOFFILE;
    	case NET$_CREF:
    	    return SS$_REJECT;
    	default:
    	    return SS$_ABORT;

    }

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

    dst->iosb_w_status = netlib___cvt_status(src);
    dst->iosb_w_count = src->iosb_w_count;
    dst->iosb_l_unused = 0;

} /* netlib___cvt_iosb */

/*
**++
**  ROUTINE:	netlib_dns_mxlookup
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
    struct IOR *ior;
    struct NETLIBIOSBDEF myiosb;
    struct CMU_RR rr_info;
    char *namp, nambuf[256];
    unsigned short namlen;
    unsigned int status;
    int argc;

    VERIFY_CTX(xctx, ctx);
    SETARGCOUNT(argc);

    if (argc < 4) return SS$_INSFARG;
    if (namdsc == 0 || mxrr == 0 || mxrrsize == 0) return SS$_BADPARAM;

    if (bypass_cmu_resolver()) {
    	return netlib___dns_mx_lookup(xctx, namdsc, mxrr, mxrrsize,
    	    	(argc > 4) ? mxrrcount : 0, (argc > 5) ? iosb : 0,
    	    	(argc > 6) ? astadr : 0, (argc > 7) ? astprm : 0);
    }

    status = lib$analyze_sdesc(namdsc, &namlen, &namp);
    if (!OK(status)) return status;
    if (namlen > sizeof(nambuf)-2) namlen = sizeof(nambuf)-2;
    memcpy(nambuf, namp, namlen);
    if (memchr(nambuf, '.', namlen) != 0) {
    	if (nambuf[namlen-1] != '.') nambuf[namlen++] = '.';
    }
    nambuf[namlen] = '\0';
    
    if (argc > 6 && astadr != 0) {
    	GET_IOR(ior, ctx, iosb, astadr, (argc > 6) ? astprm : 0);
    	status = lib$get_vm(&rr_info_size, &ior->spec_rr_info);
    	if (!OK(status)) {
    	    FREE_IOR(ior);
    	    return status;
    	}
    	ior->spec_userbuf    = mxrr;
    	ior->spec_length = *mxrrsize;
    	ior->spec_retlen = mxrrcount;
    	ior->iorflags = IOR_M_COPY_MXRRS;
    	status = sys$qio(netlib_asynch_efn, ctx->chan, IO__GTHST, &ior->iosb,
    	    	    	io_completion, ior, ior->spec_rr_info,
    	    	    	rr_info_size,
    	    	    	CMU_K_GTHST_DNSLOOKUP,
    	    	    	nambuf, NETLIB_K_DNS_TYPE_MX, 0);
    	if (!OK(status)) {
    	    lib$free_vm(&rr_info_size, &ior->spec_rr_info);
    	    FREE_IOR(ior);
    	}
    	return status;
    }

    status = sys$qiow(netlib_synch_efn, ctx->chan, IO__GTHST, &myiosb,
    	    	    	0, 0, &rr_info, sizeof(rr_info),
    	    	    	CMU_K_GTHST_DNSLOOKUP,
    	    	    	nambuf, NETLIB_K_DNS_TYPE_MX, 0);
    if (OK(status)) status = netlib___cvt_status(&myiosb);
    if (argc > 5 && iosb != 0) netlib___cvt_iosb(iosb, &myiosb);
    if (OK(status)) {
    	unsigned int i, len;
    	int remain;
    	unsigned char *cp;
    	cp = rr_info.rr_x_data + 1;
    	remain = rr_info.rr_w_length - 1;
    	i = 0;
    	while (remain > 0 && i < *mxrrsize) {
    	    len = *(unsigned short *)cp; cp += 2;
    	    mxrr[i].mxrr_l_preference = *(unsigned short *) cp;
    	    expand(mxrr[i].mxrr_t_name, NETLIB_S_MXRR_NAME, cp + 2,
    	    	    	&mxrr[i].mxrr_l_length);
    	    remain -= len + 2;
    	    cp += len;
    	    i++;
    	}
    	if (argc > 4 && mxrrcount != 0) *mxrrcount = i;
    }

    return status;

} /* netlib_dns_mx_lookup */

/*
**++
**  ROUTINE:	x
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
static void expand (char *out, unsigned int maxlen,
    	    	    unsigned char *in, unsigned int *len) {

    unsigned char *cp;
    char *cp1;
    unsigned int i;

    cp1 = out;
    cp = in;

    while ((cp1-out) < maxlen) {
    	i = *cp++;
    	if (i == 0) break;
    	if (cp1 != out) *cp1++ = '.';
    	if ((cp1-out) + i > maxlen) break;
    	memcpy(cp1, cp, i);
    	cp1 += i;
    	cp += i;
    }

    *len = (cp1 - out);

} /* expand */

/*
**++
**  ROUTINE:	bypass_cmu_resolver
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
static int bypass_cmu_resolver (void) {

    static unsigned int zero = 0;
    static $DESCRIPTOR(tabnam, "LNM$FILE_DEV");
    static $DESCRIPTOR(lognam, "NETLIB_BYPASS_CMU_RESOLVER");

    return OK(sys$trnlnm(0, &tabnam, &lognam, 0, &zero));

} /* bypass_cmu_resolver */
