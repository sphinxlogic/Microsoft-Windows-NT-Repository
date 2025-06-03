#ifdef __DECC
#pragma module MODULE_NAME
#else
#module MODULE_NAME
#endif
/*
**++
**  FACILITY:	NETLIB
**
**  ABSTRACT:	Routines specific to MultiNet/TCPware/PathWay.
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
**  	02-AUG-1995 V1.0-1  Madison 	Fix for PathWay.
**  	12-OCT-1995 V1.0-2  Madison 	Another fix for PathWay.
**--
*/
#include "netlib_sri.h"
#include "netlib.h"
#include <psldef.h>
#include <netdb.h>
#include <lnmdef.h>

#if defined(MULTINET)
#include "netlib_multinet.h"
#elif defined(TCPWARE)
#include "netlib_tcpware.h"
#elif defined(PATHWAY)
#include "netlib_pathway.h"
#endif
/*
**  Forward declarations
*/
    unsigned int netlib_get_hostname(void *bufdsc, unsigned short *retlenp);
    unsigned int netlib___ht_name_to_addr(struct CTX *ctx,
    	    	struct dsc$descriptor *namdsc, struct INADDRDEF *addrlist,
    	    	unsigned int listsize, unsigned int *count,
    	    	struct NETLIBIOSBDEF *iosb,
    	    	void (*astadr), void *astprm);
    unsigned int netlib___ht_addr_to_name(struct CTX *ctx,
    	    	struct INADDRDEF *addr, unsigned int addrsize,
    	    	struct dsc$descriptor *bufdsc, unsigned short *retlen,
    	    	struct NETLIBIOSBDEF *iosb,
    	    	void (*astadr), void *astprm);
    int netlib___get_nameservers(QUEUE *nsq);
    int netlib___get_domain(char *, unsigned short, unsigned short *);
/*
**  OWN storage
*/
    static $DESCRIPTOR(nameserver_tabnam, "LNM$FILE_DEV");

/*
**  External references
*/
    unsigned int netlib___find_symbol(char *imgnam, char *symnam, void *symptr);
    unsigned int netlib_strtoaddr(struct dsc$descriptor *dsc, struct INADDRDEF *a);

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
    unsigned int status;
    unsigned short retlen;
    static int (*gethostname)() = 0;
    int argc;

    SETARGCOUNT(argc);

    if (gethostname == 0) {
    	int i, j;
    	for (i = 0; i < sizeof(socket_library)/sizeof(socket_library[0]); i++) {
    	    for (j = 0; j < sizeof(gethostname_ent)/sizeof(gethostname_ent[0]); j++) {
    	    	status = netlib___find_symbol(socket_library[i],
    	    	    	    	gethostname_ent[j], &gethostname);
    	    	if (OK(status)) break;
    	    }
    	}
    	if (!OK(status)) return status;
    }
    if ((*gethostname)(buf, sizeof(buf)) < 0) return SS$_ABORT;
    retlen = strlen(buf);

    status = str$copy_r(bufdsc, &retlen, buf);
    if (!OK(status)) return status;
    if (argc > 1 && retlenp != 0) *retlenp = retlen;

    return SS$_NORMAL;

} /* netlib_get_hostname */

/*
**++
**  ROUTINE:	netlib___ht_name_to_addr
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
unsigned int netlib___ht_name_to_addr (struct CTX *ctx,
    	    	struct dsc$descriptor *bufdsc, struct INADDRDEF *addrlist,
    	    	unsigned int listsize, unsigned int *count,
    	    	struct NETLIBIOSBDEF *iosb,
    	    	void (*astadr), void *astprm) {

    
    char *namp, name[256];
    unsigned short namlen;
    unsigned int status;
    struct hostent *hp;
    static struct hostent *(*_gethostbyname)() = 0;
    int argc, i, j;

    if (lib$ast_in_prog()) return SS$_UNSUPPORTED;

    SETARGCOUNT(argc);

    status = lib$analyze_sdesc(bufdsc, &namlen, &namp);
    if (!OK(status)) return status;

    if (namlen > sizeof(name)-1) namlen = sizeof(name)-1;
    memcpy(name, namp, namlen);
    name[namlen] = '\0';

    if (_gethostbyname == 0) {
    	for (i = 0; i < sizeof(socket_library)/sizeof(socket_library[0]); i++) {
    	    for (j = 0; j < sizeof(_gethostbyname_ent)/sizeof(_gethostbyname_ent[0]); j++) {
    	    	status = netlib___find_symbol(socket_library[i],
    	    	    	    	_gethostbyname_ent[j], &_gethostbyname);
    	    	if (OK(status)) break;
    	    }
    	}
    	if (!OK(status)) return status;
    }

    hp = (*_gethostbyname)(name);
    if (hp == 0) return SS$_ENDOFFILE;

    for (i = 0, j = 0; (j < listsize) && (hp->h_addr_list[i] != 0); i++, j++) {
    	addrlist[j] = *((struct INADDRDEF *) hp->h_addr_list[i]);
    }
    if (argc > 4 && count != 0) *count = j;

    if (argc > 5 && iosb != 0) {
    	iosb->iosb_w_status = SS$_NORMAL;
    	iosb->iosb_w_count = j;
    	iosb->iosb_l_unused = 0;
    }

    if (argc > 6 && astadr != 0) sys$dclast(astadr, (argc > 7) ? astprm : 0, 0);

    return SS$_NORMAL;

} /* netlib___ht_name_to_addr */

/*
**++
**  ROUTINE:	netlib___ht_addr_to_name
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
unsigned int netlib___ht_addr_to_name (struct CTX *ctx,
    	    	struct INADDRDEF *addr, unsigned int addrsize,
    	    	struct dsc$descriptor *bufdsc, unsigned short *retlen,
    	    	struct NETLIBIOSBDEF *iosb,
    	    	void (*astadr), void *astprm) {

    unsigned int status;
    unsigned short len;
    struct hostent *hp;
    static struct hostent *(*_gethostbyaddr)() = 0;
    int argc;

    if (lib$ast_in_prog()) return SS$_UNSUPPORTED;

    SETARGCOUNT(argc);

    if (_gethostbyaddr == 0) {
    	int i, j;
    	for (i = 0; i < sizeof(socket_library)/sizeof(socket_library[0]); i++) {
    	    for (j = 0; j < sizeof(_gethostbyaddr_ent)/sizeof(_gethostbyaddr_ent[0]); j++) {
    	    	status = netlib___find_symbol(socket_library[i],
    	    	    	    	_gethostbyaddr_ent[j], &_gethostbyaddr);
    	    	if (OK(status)) break;
    	    }
    	}
    	if (!OK(status)) return status;
    }

    hp = (*_gethostbyaddr)(addr, addrsize, NETLIB_K_AF_INET);
    if (hp == 0) return SS$_ENDOFFILE;

    len = strlen(hp->h_name);
    status = str$copy_r(bufdsc, &len, hp->h_name);
    if (!OK(status)) return status;

    if (argc > 4 && retlen != 0) *retlen = len;

    if (argc > 5 && iosb != 0) {
    	iosb->iosb_w_status = SS$_NORMAL;
    	iosb->iosb_w_count = len;
    	iosb->iosb_l_unused = 0;
    }

    if (argc > 6 && astadr != 0) sys$dclast(astadr, (argc > 7) ? astprm : 0, 0);

    return SS$_NORMAL;

} /* netlib___ht_addr_to_name */

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
    struct dsc$descriptor dsc, lognamdsc;
    ITMLST lnmlst[4];
    char name[256], *cp, *anchor;
    unsigned int status, size, maxidx;
    unsigned short namlen;
    int index, i, remain;
    static unsigned int ns_size = sizeof(struct NAMESERVER);
    static $DESCRIPTOR(localhost, "127.0.0.1");

    ITMLST_INIT(lnmlst[0], LNM$_INDEX, sizeof(index), &index, 0);
    ITMLST_INIT(lnmlst[1], LNM$_STRING, sizeof(name), name, &namlen);
    ITMLST_INIT(lnmlst[2], LNM$_MAX_INDEX, sizeof(maxidx), &maxidx, 0);
    ITMLST_INIT(lnmlst[3], 0, 0, 0, 0);

#ifdef PATHWAY
/*
**  For PathWay, we're only supposed to use DNS if INET_DOMAIN_NAME is
**  defined
*/
    if (!OK(sys$trnlnm(0, &nameserver_tabnam, &domain_lognam, 0, &lnmlst[3]))) {
    	return 0;
    }
#endif /* PATHWAY */

    for (i = 0; i < sizeof(nameserver_lognam)/sizeof(nameserver_lognam[0]); i++) {
    	INIT_SDESC(lognamdsc, strlen(nameserver_lognam[i]), nameserver_lognam[i]);
    	status = sys$trnlnm(0, &nameserver_tabnam, &lognamdsc, 0, &lnmlst[2]);
    	if (OK(status)) break;
    }

    if (!OK(status)) {
#ifdef PATHWAY
/*
**  For PathWay, if INET_NAMESERVER_LIST is not defined at all, but
**  INET_DOMAIN_NAME is defined, we're supposed to assume that the
**  server is the local host.
*/
    	status = lib$get_vm(&ns_size, &ns);
    	if (OK(status)) {
    	    netlib_strtoaddr((struct dsc$descriptor *) &localhost, &ns->addr);
    	    queue_insert(ns, nsq->tail);
    	    return 1;
    	}
#endif /* PATHWAY */
    	return 0;
    }

    ITMLST_INIT(lnmlst[2], 0, 0, 0, 0);

    for (index = 0; index <= maxidx; index++) {
    	status = sys$trnlnm(0, &nameserver_tabnam, &lognamdsc, 0, lnmlst);
    	if (!OK(status) || namlen == 0) break;
    	for (anchor = name, remain = namlen; remain > 0;
    	    	    	    	    	remain -= i+1, anchor = cp+1) {
/*
**  PathWay separates addresses with blanks, not commas.
*/
    	    cp = memchr(anchor, ',', remain);
    	    if (cp == 0) cp = memchr(anchor, ' ', remain);
    	    if (cp == 0) i = remain;
    	    else i = cp - anchor;
    	    INIT_SDESC(dsc, i, anchor);
/*
**  PathWay uses 0.0.0.0 in place of 127.0.0.1 to refer to the server
**  running on the local host.
*/
    	    if (i == 7 && memcmp(anchor, "0.0.0.0", i) == 0) {
    	    	netlib_strtoaddr((struct dsc$descriptor *) &localhost, &a);
    	    } else {
    	    	if (!OK(netlib_strtoaddr(&dsc, &a))) continue;
    	    }
    	    status = lib$get_vm(&ns_size, &ns);
    	    if (!OK(status)) break;
    	    ns->addr = a;
    	    queue_insert(ns, nsq->tail);
    	}
    }


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

    struct dsc$descriptor dsc, lognamdsc;
    ITMLST lnmlst[2];
    char name[256], *cp;
    unsigned int status;
    unsigned short namlen;
    int i;

    ITMLST_INIT(lnmlst[0], LNM$_STRING, sizeof(name), name, &namlen);
    ITMLST_INIT(lnmlst[1], 0, 0, 0, 0);

    for (i = 0; i < sizeof(domain_lognam)/sizeof(domain_lognam[0]); i++) {
    	INIT_SDESC(lognamdsc, strlen(domain_lognam[i]), domain_lognam[i]);
    	status = sys$trnlnm(0, &nameserver_tabnam, &lognamdsc, 0, lnmlst);
    	if (OK(status)) {
    	    cp = name;
    	    break;
    	}
    }

    if (!OK(status)) {
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
