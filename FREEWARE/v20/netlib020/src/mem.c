/*
**++
**  FACILITY:	NETLIB
**
**  ABSTRACT:	Memory management routines
**
**  MODULE DESCRIPTION:
**
**  	tbs
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1994, MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  02-APR-1994
**
**  MODIFICATION HISTORY:
**
**  	02-APR-1994 V1.0    Madison 	Initial coding.
**  	09-NOV-1994 V1.1    Madison 	Add IOR, DNSREQ.
**  	31-AUG-1995 V1.1-1  Madison 	Update free_ctx() to free wlinebuf.
**--
*/
#define __NETLIB_MODULE_MEM__
#include "netlib.h"
#include <libvmdef.h>
/*
** Forward declarations
*/
    unsigned int netlib___alloc_ctx(struct CTX **, unsigned int);
    unsigned int netlib___free_ctx(struct CTX *);
    unsigned int netlib___alloc_ior(struct IOR **);
    unsigned int netlib___free_ior(struct IOR *);
    unsigned int netlib___alloc_dnsreq(struct DNSREQ **);
    unsigned int netlib___free_dnsreq(struct DNSREQ *);
/*
** OWN and GLOBAL storage
*/
    static unsigned int ctxzone = 0;
    static unsigned int CTX_S_CTXDEF = sizeof(struct CTX);
    static unsigned int iorzone = 0;
    static unsigned int IOR_S_IORDEF = sizeof(struct IOR);
    static unsigned int dnsreqzone = 0;
    static unsigned int DNSREQ_S_DNSREQDEF = sizeof(struct DNSREQ);
#pragma nostandard
    globaldef unsigned int netlib_synch_efn = 0xffffffff;
    globaldef unsigned int netlib_asynch_efn = 0xffffffff;
#pragma standard
    static QUEUE iorque = {&iorque, &iorque};
    static QUEUE dnsreqque = {&dnsreqque, &dnsreqque};

/*
**  External references
*/
    void netlib___free_dns_context(struct CTX *);


/*
**++
**  ROUTINE:	netlib___alloc_ctx
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates some memory.
**
**  RETURNS:	cond_value
**
**  PROTOTYPE:
**
**  	netlib___alloc_ctx(unsigned int size, void *ptr)
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
unsigned int netlib___alloc_ctx (struct CTX **ctxp, unsigned int specsize) {

    struct CTX *ctx;
    unsigned int status, fullsize, aststat;

    aststat = sys$setast(0);
    if (netlib_synch_efn == 0xffffffff) {
    	status = lib$get_ef(&netlib_synch_efn);
    	if (!OK(status)) {
    	    if (aststat == SS$_WASSET) sys$setast(1);
    	    return status;
    	}
    }
    if (netlib_asynch_efn == 0xffffffff) {
    	status = lib$get_ef(&netlib_asynch_efn);
    	if (!OK(status)) {
    	    if (aststat == SS$_WASSET) sys$setast(1);
    	    return status;
    	}
    }

    fullsize = specsize + CTX_S_CTXDEF;
    if (ctxzone == 0) {
    	unsigned int algorithm=LIB$K_VM_FIXED;
    	unsigned int flags=LIB$M_VM_GET_FILL0|LIB$M_VM_EXTEND_AREA;
    	status = lib$create_vm_zone(&ctxzone, &algorithm, &fullsize, &flags);
    	if (!OK(status)) {
    	    if (aststat == SS$_WASSET) sys$setast(1);
    	    return status;
    	}
    }

    if (aststat == SS$_WASSET) sys$setast(1);

    status = lib$get_vm(&fullsize, &ctx, &ctxzone);
    if (OK(status)) {
    	ctx->specctx = ctx + 1;
    	ctx->specctx_size = specsize;
    	if (!OK(status)) lib$free_vm(&fullsize, &ctx, &ctxzone);
    	*ctxp = ctx;
    	return SS$_NORMAL;
    }

    return status;

} /* netlib___alloc_ctx */

/*
**++
**  ROUTINE:	netlib___free_ctx
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a block of memory.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	netlib___free_ctx(struct CMD *c)
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
unsigned int netlib___free_ctx (struct CTX *ctx) {

    unsigned int fullsize;

    if (ctx->linebuf != 0) {
    	fullsize = CTX_S_LINEBUF;
    	lib$free_vm(&fullsize, &ctx->linebuf);
    }

    if (ctx->wlinebuf != 0 && ctx->wlinesize != 0) {
    	lib$free_vm(&ctx->wlinesize, &ctx->wlinebuf);
    }

    netlib___free_dns_context(ctx);

    fullsize = ctx->specctx_size + CTX_S_CTXDEF;
    return lib$free_vm(&fullsize, &ctx, &ctxzone);

} /* netlib___free_ctx */

/*
**++
**  ROUTINE:	netlib___alloc_ior
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates some memory.
**
**  RETURNS:	cond_value
**
**  PROTOTYPE:
**
**  	netlib___alloc_ior(unsigned int size, void *ptr)
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
unsigned int netlib___alloc_ior (struct IOR **iorp) {

    struct IOR *ior;
    unsigned int status, fullsize;

    if (iorzone == 0) {
    	unsigned int algorithm=LIB$K_VM_FIXED;
    	unsigned int flags=LIB$M_VM_GET_FILL0|LIB$M_VM_EXTEND_AREA;
    	int i;
    	status = lib$create_vm_zone(&iorzone, &algorithm, &IOR_S_IORDEF, &flags);
    	if (!OK(status)) return status;
    	for (i = 0; i < 8; i++) {
    	    if (!OK(lib$get_vm(&IOR_S_IORDEF, &ior))) break;
    	    queue_insert(ior, iorque.tail);
    	}
    }

    if (queue_remove(iorque.head, &ior)) {
    	memset(ior, 0, IOR_S_IORDEF);
    	*iorp = ior;
    	return SS$_NORMAL;
    }

    status = lib$get_vm(&IOR_S_IORDEF, &ior, &iorzone);
    if (OK(status)) *iorp = ior;

    return status;

} /* netlib___alloc_ior */

/*
**++
**  ROUTINE:	netlib___free_ior
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a block of memory.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	netlib___free_ior(struct CMD *c)
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
unsigned int netlib___free_ior (struct IOR *ior) {

    queue_insert(ior, iorque.tail);

    return SS$_NORMAL;

} /* netlib___free_ior */

/*
**++
**  ROUTINE:	netlib___alloc_dnsreq
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates some memory.
**
**  RETURNS:	cond_value
**
**  PROTOTYPE:
**
**  	netlib___alloc_dnsreq(unsigned int size, void *ptr)
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
unsigned int netlib___alloc_dnsreq (struct DNSREQ **dnsreqp) {

    struct DNSREQ *dnsreq;
    unsigned int status, fullsize;

    if (dnsreqzone == 0) {
    	unsigned int algorithm=LIB$K_VM_FIXED;
    	unsigned int flags=LIB$M_VM_GET_FILL0|LIB$M_VM_EXTEND_AREA;
    	status = lib$create_vm_zone(&dnsreqzone, &algorithm, &DNSREQ_S_DNSREQDEF, &flags);
    	if (!OK(status)) return status;
    }

    if (queue_remove(dnsreqque.head, &dnsreq)) {
    	memset(dnsreq, 0, DNSREQ_S_DNSREQDEF);
    	*dnsreqp = dnsreq;
    	return SS$_NORMAL;
    }

    status = lib$get_vm(&DNSREQ_S_DNSREQDEF, &dnsreq, &dnsreqzone);
    if (OK(status)) *dnsreqp = dnsreq;

    return status;

} /* netlib___alloc_dnsreq */

/*
**++
**  ROUTINE:	netlib___free_dnsreq
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a block of memory.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	netlib___free_dnsreq(struct CMD *c)
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
unsigned int netlib___free_dnsreq (struct DNSREQ *dnsreq) {

    queue_insert(dnsreq, dnsreqque.tail);

    return SS$_NORMAL;

} /* netlib___free_dnsreq */
