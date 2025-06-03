/*
**++
**  FACILITY:	NSQUERY
**
**  ABSTRACT:	Network interface for use with MultiNet.
**
**  MODULE DESCRIPTION:
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  25-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	25-SEP-1992 V1.0    Madison 	Initial coding.
**--
*/
#include "nsquery.h"
#include "multinet_root:[multinet.include.sys]types.h"
#include "multinet_root:[multinet.include.sys]socket.h"
#include "multinet_root:[multinet.include.netinet]in.h"
#include "multinet_root:[multinet.include]netdb.h"
#include <iodef.h>
#include "multinet_root:[multinet.include.vms]inetiodef.h"

    struct CTX {
    	unsigned short s;
    	int tcp;
    	int have_socket;
    	int connected;
    	short iosb[4];
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

    static $DESCRIPTOR(mn_device, "INET0:");
    unsigned int status;

    *ctx = malloc(sizeof(struct CTX));
    (*ctx)->connected = (*ctx)->tcp = (*ctx)->have_socket = 0;
    status = sys$assign(&mn_device, &((*ctx)->s), 0, 0);
    if (!OK(status)) free(*ctx);
    return status;

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

    struct sockaddr_in sin;
    unsigned int status;

    (*ctx)->tcp = use_tcp;
    if (!(*ctx)->have_socket) {
    	status = sys$qiow(0, (*ctx)->s, IO$_SOCKET, (*ctx)->iosb, 0, 0,
    	    	    AF_INET, (use_tcp ? SOCK_STREAM : SOCK_DGRAM), 0, 0, 0, 0);
    	if (OK(status)) status = (*ctx)->iosb[0];
    	if (!OK(status)) return status;
    	(*ctx)->have_socket = 1;
    }
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = (u_long) *addr;
    sin.sin_port = htons(port);
    memset(sin.sin_zero, 0, sizeof(sin.sin_zero));
    status = sys$qiow(0, (*ctx)->s, IO$_CONNECT, (*ctx)->iosb, 0, 0,
    	    	&sin, sizeof(sin), 0, 0, 0, 0);
    if (OK(status)) status = (*ctx)->iosb[0];
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

    if ((*ctx)->connected) {
    	status = sys$qiow(0, (*ctx)->s, IO$_SHUTDOWN, (*ctx)->iosb, 0, 0,
    	    	2, 0, 0, 0, 0, 0);
    }
    status = sys$dassgn((*ctx)->s);
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

    status = sys$qiow(0, (*ctx)->s, IO$_WRITEVBLK, (*ctx)->iosb, 0, 0,
    	    	buf, bufsize, 0, 0, 0, 0);
    if (OK(status)) status = (*ctx)->iosb[0];

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
    unsigned int status;

    if ((*ctx)->tcp) {
    	*buflen = 0;
    	while (*buflen < 2) {
    	    status = sys$qiow(0, (*ctx)->s, IO$_READVBLK, (*ctx)->iosb, 0, 0,
    	    	    (unsigned char *)buf+*buflen, bufsize-*buflen, 0, 0, 0, 0);
    	    if (OK(status)) status = (*ctx)->iosb[0];
    	    if (!OK(status)) return status;
    	    *buflen += (*ctx)->iosb[1];
    	}
    	rcvlen = (*(unsigned char *)buf)<<8 + *((unsigned char *)buf+1);
    	if (rcvlen > bufsize-2) rcvlen = bufsize-2;
    	while (*buflen < rcvlen+2) {
    	    status = sys$qiow(0, (*ctx)->s, IO$_READVBLK, (*ctx)->iosb, 0, 0,
    	    	    (unsigned char *)buf+*buflen, bufsize-*buflen, 0, 0, 0, 0);
    	    if (OK(status)) status = (*ctx)->iosb[0];
    	    if (!OK(status)) return status;
    	    *buflen += (*ctx)->iosb[1];
    	}
    	return SS$_NORMAL;
    }

    status = sys$setimr(0, tmo, tmo_ast, (*ctx)->s, 0);
    if (!OK(status)) return status;
    status = sys$qio(0, (*ctx)->s, IO$_READVBLK, (*ctx)->iosb,
    	    	qio_ast, (*ctx)->s,
    	    	(short *)buf+1, bufsize-sizeof(short), 0, 0, 0, 0);
    if (!OK(status)) {
    	sys$cantim((*ctx)->s);
    	return status;
    }
    status = sys$synch(0, (*ctx)->iosb);
    if (OK(status)) status = (*ctx)->iosb[0];
    if (OK(status)) *buflen = (*ctx)->iosb[1];
    return status;

} /* network_response */

/*
**++
**  ROUTINE:	qio_ast
**
**  FUNCTIONAL DESCRIPTION:
**
**  	AST completion routine for a UDP QIO read.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	qio_ast(short s)
**
**  s:	channel, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL always returned.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static unsigned int qio_ast(short s) {

    sys$cantim(s);
    return SS$_NORMAL;

} /* qio_ast */

/*
**++
**  ROUTINE:	tmo_ast
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Timer AST routine.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	tmo_ast(short s)
**
**  s:	channel, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL always returned.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static unsigned int tmo_ast(short s) {

    sys$cancel(s);
    return SS$_NORMAL;

} /* tmo_ast */

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

    gethostname(buf, bufsize);

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
    struct sockaddr_in *sa;
    struct hostent *srvh;

    if (strspn(name, "0123456789.") == strlen(name)) {
    	adr = malloc(sizeof(struct ADR));
    	adr->address = inet_addr(name);
    	INSQUE(adr, adrque->tail);
    	return;
    }
    srvh = gethostbyname(name);
    if (srvh == NULL) lib$stop(NSQ__SRVNAMERR, 1, name);
    for (sa = (struct sockaddr_in *) srvh->h_addresses;
    	    sa->sin_family == AF_INET; sa++) {
    	adr = malloc(sizeof(struct ADR));
    	adr->address = (unsigned int) (sa->sin_addr.s_addr);
    	INSQUE(adr, adrque->tail);
    }
    if (adrque->head == adrque) {
    	lib$stop(NSQ__SRVNAMERR, 1, name);
    }
} /* network_lookup */
