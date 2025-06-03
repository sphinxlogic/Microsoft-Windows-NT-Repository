/*
**++
**  FACILITY:	NETLIB
**
**  ABSTRACT:	DNS resolver support.
**
**  MODULE DESCRIPTION:
**
**  	This module contains support routines for implementing NETLIB's
**  Domain Name Service resolver.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1994, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  22-OCT-1994
**
**  MODIFICATION HISTORY:
**
**  	22-OCT-1994 V1.0    Madison 	Initial coding.
**  	17-NOV-1994 V1.1    Madison 	Removed socket from DNS context.
**  	18-NOV-1994 V1.2    Madison 	Add retry count.
**--
*/
#include "netlib.h"
#include <lnmdef.h>
/*
**  Forward declarations
*/
    unsigned int netlib___dns_init(struct CTX *ctx);
    void netlib___free_dns_context(struct CTX *ctx);
    int netlib_dns_skipname(unsigned char *bufp, unsigned short *buflen);
    unsigned int netlib_dns_expandname(unsigned char *buf, unsigned short *buflen,
    	    	    unsigned char *bufp, struct dsc$descriptor *namdsc,
    	    	    unsigned short *retlen, unsigned short *bufchrs);
/*
**  OWN storage
*/
    static $DESCRIPTOR(default_timeout, "0 00:00:05");

/*
**  External references
*/
    int netlib___get_nameservers(QUEUE *nsq);
    int netlib___get_domain(char *, unsigned short, unsigned short *);

/*
**++
**  ROUTINE:	netlib___dns_init
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Initializes an internal DNS resolver context.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	netlib___dns_init(struct CTX *ctx)
**
**  ctx:    NETLIB context, modify, by reference.
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	See code.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int netlib___dns_init (struct CTX *ctx) {

    struct DOMAIN *dom;
    unsigned int status;
    ITMLST lnmlst[3];
    int i, maxidx;
    unsigned int size;
    unsigned short buflen;
    char buf[256];

    static unsigned int socktype = NETLIB_K_TYPE_DGRAM;
    static unsigned int ctxsize = sizeof(struct DNSCTX);
    static $DESCRIPTOR(tabnam, "LNM$FILE_DEV");
    static $DESCRIPTOR(lognam, "NETLIB_SEARCH_DOMAIN");

/*
**  Allocate the DNS resolver context
*/
    status = lib$get_vm(&ctxsize, &ctx->dnsctx);
    if (!OK(status)) return status;

    INIT_QUEUE(ctx->dnsctx->nsq);
    INIT_QUEUE(ctx->dnsctx->domq);

/*
**  Get the list of name servers we're supposed to contact
*/
    if (netlib___get_nameservers(&ctx->dnsctx->nsq) == 0) {
    	lib$free_vm(&ctxsize, &ctx->dnsctx);
    	ctx->dnsctx = 0;
    	ctx->flags |= CTX_M_NO_DNS;
    	return SS$_UNSUPPORTED;
    }

/*
**  Get the list of NETLIB search domains, or the package-specific
**  ones.
*/
    ITMLST_INIT(lnmlst[0], LNM$_MAX_INDEX, sizeof(maxidx), &maxidx, 0);
    ITMLST_INIT(lnmlst[1], 0, 0, 0, 0);
    if (OK(sys$trnlnm(0, &tabnam, &lognam, 0, lnmlst))) {
    	ITMLST_INIT(lnmlst[0], LNM$_INDEX, sizeof(i), &i, 0);
    	ITMLST_INIT(lnmlst[1], LNM$_STRING, sizeof(buf), buf, &buflen);
    	ITMLST_INIT(lnmlst[2], 0, 0, 0, 0);
    	for (i = 0; i <= maxidx; i++) {
    	    if (OK(sys$trnlnm(0, &tabnam, &lognam, 0, lnmlst)) && buflen != 0) {
    	    	size = buflen + sizeof(struct DOMAIN);
    	    	if (OK(lib$get_vm(&size, &dom))) {
    	    	    dom->length = buflen;
    	    	    memcpy(dom->name, buf, buflen);
    	    	    dom->name[buflen] = '\0';
    	    	    queue_insert(dom, ctx->dnsctx->domq.tail);
    	    	}
    	    }
    	}
    } else if (netlib___get_domain(buf, sizeof(buf), &buflen)) {
    	char *cp, *cp1;
    	int remain;

    	cp = buf;
    	remain = buflen;
/*
**  A search domain must have at least two parts, to avoid the ".com.edu"
**  problem, which is why we check to make sure that the remaining domain
**  string has at least one dot.
*/
    	while (remain > 0) {
    	    cp1 = memchr(cp, '.', remain);
    	    if (cp1 == 0) break;
    	    size = remain + sizeof(struct DOMAIN);
    	    if (OK(lib$get_vm(&size, &dom))) {
    	    	dom->length = remain;
    	    	memcpy(dom->name, cp, remain);
    	    	dom->name[remain] = '\0';
    	    	queue_insert(dom, ctx->dnsctx->domq.tail);
    	    }
    	    cp = cp1 + 1;
    	    remain -= (cp1 - cp) + 1;
    	}
    }

    sys$bintim(&default_timeout, &ctx->dnsctx->timeout);
    ctx->dnsctx->retry_count = 4;

    return SS$_NORMAL;

} /* netlib___dns_init */    

/*
**++
**  ROUTINE:	netlib___free_dns_context
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Deallocates the resolver context.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	netlib___free_dns_context(struct CTX *ctx)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	See code.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void netlib___free_dns_context (struct CTX *ctx) {

    struct NAMESERVER *ns;
    struct DOMAIN *d;
    unsigned int size;

    if (ctx->dnsctx == 0) return;

    size = sizeof(struct NAMESERVER);
    while (queue_remove(ctx->dnsctx->nsq.head, &ns)) lib$free_vm(&size, &ns);
    while (queue_remove(ctx->dnsctx->domq.head, &d)) {
    	size = sizeof(struct DOMAIN) + d->length;
    	lib$free_vm(&size, &d);
    }

    size = sizeof(struct DNSCTX);
    lib$free_vm(&size, &ctx->dnsctx);
    ctx->dnsctx = 0;

} /* netlib___free_dns_context */

/*
**++
**  ROUTINE:	netlib_dns_skipname
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Utility routine for skipping over a name in a DNS response.
**  Handles the compressed form of domain string that DNS packets use.
**
**  RETURNS:	longword_signed
**
**  	The number of bytes to be skipped in the DNS packet is returned.
**
**  PROTOTYPE:
**
**  	NETLIB_DNS_SKIPNAME  bufp, buflen
**
**  bufp:   char_string, character string, read only, by reference
**  buflen: word_unsigned, word (unsigned), read only, by reference
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
int netlib_dns_skipname (unsigned char *bufp, unsigned short *buflen) {

    unsigned char *cp, *eom;

    cp = bufp;
    eom = cp + *buflen;
    while (cp < eom && *cp != 0) {
    	if (*cp >= 64) cp++;
    	else cp += *cp + 1;
    }
    return (cp - bufp) + 1;

} /* netlib_dns_skipname */

/*
**++
**  ROUTINE:	netlib_dns_expandname
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Expands a domain string from a DNS reply packet into something
**  human-readable.  The string is returned, along with the number of
**  bytes in the DNS packet that composed the name (which won't necessarily
**  be the same as the string length because of the compression used in
**  DNS packets).
**
**  RETURNS:	cond_value, condition value, write only, by value
**
**  PROTOTYPE:
**
**  	NETLIB_DNS_EXPANDNAME  buf, buflen, bufp, namdsc [,retlen] [,count]
**
**  buf:    	char_string, character string, read only, by reference
**  buflen: 	word_unsigned, word (unsigned), read only, by reference
**  bufp:   	pointer, longword (unsigned), modify, by reference
**  namdsc: 	char_string, character string, write only, by descriptor
**  retlen: 	word_unsigned, word (unsigned), write only, by reference
**  count:  	word_unsigned, word (unsigned), write only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	See code.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int netlib_dns_expandname (unsigned char *buf, unsigned short *buflen,
    	    	unsigned char *bufp, struct dsc$descriptor *namdsc,
    	    	unsigned short *retlen, unsigned short *countp) {

    int argc, indirect;
    struct dsc$descriptor dsc;
    char name[256], *namp;
    unsigned short namlen, count;
    unsigned int status;

    SETARGCOUNT(argc);

    namp = name;
    indirect = 0;
    count = 0;
    while (*bufp != 0) {
    	if (*bufp < 64) {
    	    if (!indirect) count += *bufp + 1;
    	    if (namp-name+*bufp < sizeof(name)) {
    	    	memcpy(namp, bufp+1, *bufp);
    	    	namp += *bufp;
    	    	*namp++ = '.';
    	    }
    	    bufp += *bufp + 1;
    	} else {
    	    if (!indirect) count += 2;
    	    indirect = 1;
    	    bufp = buf + (netlib_word_swap(*(unsigned short *)bufp) - 0xc000);
    	    if (bufp - buf > *buflen) return SS$_BADPARAM;
    	}
    }

    INIT_SDESC(dsc, (namp-name)-1, name);
    status = lib$scopy_dxdx(&dsc, namdsc);
    if (!OK(status)) return status;
    if (argc > 4 && retlen != 0) *retlen = namp - name - 1;
    if (argc > 5 && countp != 0) *countp = count;
    return SS$_NORMAL;

} /* netlib_dns_expandname */
