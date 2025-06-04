/*
**++
**  FACILITY:	NSQUERY
**
**  ABSTRACT:	Network interface for use with NETLIB.
**
**  MODULE DESCRIPTION:
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, 1995 MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  25-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	25-SEP-1992 V1.0    Madison 	Initial coding.
**  	09-JAN-1995 V1.1    Madison 	Update to NETLIB V2.0.
**--
*/
#include "nsquery.h"
#include "netlib_dir:netlibdef.h"

    struct CTX {
    	void *netctx;
    	int tcp;
    	int have_socket;
    	int connected;
    	struct NETLIBIOSBDEF iosb;
    	struct SINDEF sin;
    };

#pragma nostandard

    globalvalue unsigned int NSQ__SRVNAMERR;

#pragma standard

/*
** Forward declarations
*/

    unsigned int network_init(struct CTX **);
    unsigned int network_open(struct CTX **, unsigned int *, int, int);
    unsigned int network_close(struct CTX **);
    unsigned int network_send(struct CTX **, char *, int);
    unsigned int network_response(struct CTX **, void *, int, short *, TIME *);
    static unsigned int qio_ast(short), tmo_ast(short);
    void network_local_hostname(char *, int);
    void network_lookup(char *, struct QUE *);


/*
**++
**  ROUTINE:	network_init
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
unsigned int network_init(struct CTX **ctx) {

    unsigned int status;

    *ctx = malloc(sizeof(struct CTX));
    (*ctx)->connected = (*ctx)->tcp = (*ctx)->have_socket = 0;
    return SS$_NORMAL;

} /* network_init */

/*
**++
**  ROUTINE:	network_open
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
unsigned int network_open(struct CTX **ctx, unsigned int *addr, int port,
    	    	    	    	int use_tcp) {

    unsigned int status;
    static unsigned int dgtype = NETLIB_K_TYPE_DGRAM;
    static unsigned int sttype = NETLIB_K_TYPE_STREAM;
    static unsigned int sinsize = sizeof(struct SINDEF);

    (*ctx)->tcp = use_tcp;
    status = netlib_socket(&(*ctx)->netctx, use_tcp ? &sttype : &dgtype);
    if (!OK(status)) return status;
    (*ctx)->have_socket = 1;
    memset(&(*ctx)->sin, 0, sizeof((*ctx)->sin));
    (*ctx)->sin.sin_w_family = NETLIB_K_AF_INET;
    (*ctx)->sin.sin_w_port   = netlib_word_swap(port);
    (*ctx)->sin.sin_x_addr.inaddr_l_addr = *addr;
    if (use_tcp) {
    	status = netlib_connect(&((*ctx)->netctx), &(*ctx)->sin,
    	    	    	&sinsize, &(*ctx)->iosb);
    } else {
    	status = SS$_NORMAL;
    }
    (*ctx)->connected = OK(status);
    return status;

} /* network_open */

/*
**++
**  ROUTINE:	network_close
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Closes a connection and frees the context block.
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
unsigned int network_close(struct CTX **ctx) {

    unsigned int status;

    status = netlib_close(&((*ctx)->netctx));
    free(*ctx);
    return status;

} /* network_close */

/*
**++
**  ROUTINE:	network_send
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
unsigned int network_send(struct CTX **ctx, char *buf, int bufsize) {

    unsigned int status;
    static unsigned int sinsize = sizeof(struct SINDEF);
    struct dsc$descriptor bufdsc;

    INIT_SDESC(bufdsc, bufsize, buf);
    if ((*ctx)->tcp) {
    	status = netlib_write(&((*ctx)->netctx), &bufdsc);
    } else {
    	status = netlib_write(&((*ctx)->netctx), &bufdsc, &(*ctx)->sin,
    	    	    	    &sinsize);
    }

    return status;

} /* network_send */

/*
**++
**  ROUTINE:	network_response
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Reads the response from the server.  For TCP connections,
**  collects all of the response (which may take multiple I/O's).  For
**  UDP connections, does the read with a timeout (in case the response
**  got lost).
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	network_response(struct CTX **ctx, void *buf, int size,
**  	    	    	    	int *len, TIME *tmo)
**
**  s:	    channel, read only, by value
**  tcp:    boolean, read only, by value
**  buf:    unspecified, modify, by reference
**  size:   integer, read only, by value
**  len:    integer, write only, by reference
**  tmo:    date_time, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	Any I/O status codes.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int network_response(struct CTX **ctx, void *buf,
    	    	    	int bufsize, short *buflen, TIME *tmo) {

    short rcvlen;
    struct dsc$descriptor bufdsc;
    unsigned int status;

    if ((*ctx)->tcp) {
    	*buflen = 0;
    	while (*buflen < 2) {
    	    INIT_SDESC(bufdsc, bufsize-*buflen, (char *)buf+*buflen);
    	    status = netlib_read(&((*ctx)->netctx), &bufdsc, 0, 0, 0, 0,
    	    	    	    	    	&(*ctx)->iosb);
    	    if (OK(status)) status = (*ctx)->iosb.iosb_w_status;
    	    if (!OK(status)) return status;
    	    *buflen += (*ctx)->iosb.iosb_w_count;
    	}
    	rcvlen = (*(unsigned char *)buf)<<8 + *((unsigned char *)buf+1);
    	if (rcvlen > bufsize-2) rcvlen = bufsize-2;
    	while (*buflen < rcvlen+2) {
    	    INIT_SDESC(bufdsc, bufsize-*buflen, (char *)buf+*buflen);
    	    status = netlib_read(&((*ctx)->netctx), &bufdsc, 0, 0, 0, 0,
    	    	    	    	    	&(*ctx)->iosb);
    	    if (OK(status)) status = (*ctx)->iosb.iosb_w_status;
    	    if (!OK(status)) return status;
    	    *buflen += (*ctx)->iosb.iosb_w_count;
    	}
    	return SS$_NORMAL;
    }

    INIT_SDESC(bufdsc, bufsize-sizeof(short), (char *)((short *)buf+1));
    status = netlib_read(&((*ctx)->netctx), &bufdsc, 0, 0, 0, tmo,
    	    	&(*ctx)->iosb);
    if (OK(status)) status = (*ctx)->iosb.iosb_w_status;
    if (OK(status)) *buflen = (*ctx)->iosb.iosb_w_count;
    return status;

} /* network_response */

/*
**++
**  ROUTINE:	network_local_hostname
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
void network_local_hostname(char *buf, int bufsize) {

    struct dsc$descriptor dsc;
    unsigned int status;
    unsigned short retlen;

    INIT_SDESC(dsc, bufsize-1, buf);
    status = netlib_get_hostname(&dsc, &retlen);
    if (!OK(status)) retlen = 0;
    buf[retlen] = '\0';

} /* network_local_hostname */

/*
**++
**  ROUTINE:	network_lookup
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Returns a list of nameserver addresses to try.  Handles
**  both names and numeric IP addresses.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	network_lookup(unsigned char *name, struct QUE *adrque)
**
**  name:   ASCIZ_string, read only, by reference
**  adrque: QUE structure, modify, by reference
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
void network_lookup(char *name, struct QUE *adrque) {

    struct ADR *adr;
    struct INADDRDEF adrlst[32];
    unsigned int status;
    void *tmpctx;
    struct dsc$descriptor namdsc;
    int i;
    static unsigned int socktype = NETLIB_K_TYPE_STREAM;
    static unsigned int usedns   = NETLIB_K_LOOKUP_DNS;
    static unsigned int useht   = NETLIB_K_LOOKUP_HOST_TABLE;
    unsigned int listsize, adrcnt;

    INIT_SDESC(namdsc, strlen(name), name);
    if (OK(netlib_strtoaddr(&namdsc, &adrlst[0]))) {
    	adr = malloc(sizeof(struct ADR));
    	adr->address = adrlst[0].inaddr_l_addr;
    	INSQUE(adr, adrque->tail);
    	return;
    }

    status = netlib_socket(&tmpctx, &socktype);
    listsize = sizeof(adrlst)/sizeof(unsigned int);
    status = netlib_name_to_address(&tmpctx, &usedns, &namdsc, adrlst,
    	    	&listsize, &adrcnt);
    if (!OK(status)) netlib_name_to_address(&tmpctx, &useht, &namdsc, adrlst,
    	    	&listsize, &adrcnt);
    netlib_close(&tmpctx);
    if (OK(status)) {
    	for (i = 0; i < adrcnt; i++) {
    	    adr = malloc(sizeof(struct ADR));
    	    adr->address = adrlst[i].inaddr_l_addr;
    	    INSQUE(adr, adrque->tail);
    	}
    } else {
    	lib$stop(NSQ__SRVNAMERR, 1, name, status);
    }

} /* network_lookup */
