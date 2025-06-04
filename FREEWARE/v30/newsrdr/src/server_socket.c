/*
!++
! MODULE:           SERVER_SOCKET
!
! FACILITY: 	    NEWSRDR
!
! ABSTRACT: 	    Socket-based SERVER routines.
!
! MODULE DESCRIPTION:
!
!   This module is a drop-in replacement for the standard NETLIB-based
!   SERVER module.  It uses Berkeley socket routines instead of NETLIB
!   routines and should be compatible with any TCP/IP that supports
!   sockets.
!
! AUTHOR:   	    M. Madison
!   	    	    COPYRIGHT © 1993, MADGOAT SOFTWARE.
!   	    	    ALL RIGHTS RESERVED.
!
! CREATION DATE:    25-FEB-1991
!
! MODIFICATION HISTORY:
!
!   25-FEB-1991	V1.0	Madison	    Initial coding.
!   15-MAY-1991	V1.1	Madison	    Include mods for Process TCPware.
!                                   (Martin Egger, Univ. of Bern)
!   26-AUG-1991	V1.1-1	Madison	    Fix zero-length send problem.
!   25-AUG-1992	V1.1-2	Madison	    Increasing buffering.
!   08-SEP-1992	V2.0	Madison	    Update for NewsRdr V4.0.
!   15-DEC-1992	V2.0-1	Madison	    Update for TCPware V3.0 (new logicals).
!   17-FEB-1993	V2.0-2	Madison	    Update for new message formats.
!   23-APR-1993	V2.1	Madison	    New server_check routine.
!   03-JUN-1993	V2.1-1	Madison	    Got time check backwards!
!   21-SEP-1993	V2.1-2	Madison	    Fix htons declaration.
!   07-OCT-1993	V2.1-3	Madison	    Slight adjustment for TCPware decl's.
!   09-OCT-1993	V2.1-4	Madison	    Change recv error check to just < 0.
!   10-NOV-1993	V2.1-5	Madison	    Fix ACCVIO after reconnect.
!   25-MAY-1994	V2.2	Madison	    Allow echo output to be put to other term.
!--
*/

#include <descrip.h>

#ifdef MULTINET
#include "multinet_root:[multinet.include.sys]types.h"
#include "multinet_root:[multinet.include.sys]socket.h"
#include "multinet_root:[multinet.include.netinet]in.h"
#ifdef __DECC
#define noshare
#endif
#include "multinet_root:[multinet.include]netdb.h"
#ifdef noshare
#undef noshare
#endif
#else
#ifdef TCPWARE
#include "tcpip_include:types.h"
#include "tcpip_include:socket.h"
#include "tcpip_include:in.h"
#include "tcpip_include:netdb.h"
#define send socket_send
#define recv socket_recv
#else
#ifdef TCPWARE_V30
#include "tcpware_include:types.h"
#include "tcpware_include:socket.h"
#include "tcpware_include:inet.h"
#include "tcpware_include:in.h"
#include "tcpware_include:netdb.h"
#define send socket_send
#define recv socket_recv
#else
#include <types.h>
#include <socket.h>
#include <in.h>
#include <netdb.h>
#endif
#endif
#endif
#include <lib$routines.h>
#include <str$routines.h>
#include <starlet.h>
#include <iodef.h>
#include <ssdef.h>
#include <stsdef.h>
#include <stdlib.h>
#include <string.h>

#define PUT_OUTPUT(x) {\
    if (echo_chan != 0) sys$qiow(0, echo_chan, IO$_WRITEVBLK,\
    	0, 0, 0, x, strlen(x), 0, 0x8d010000, 0, 0);\
    else put_output(x);}

#pragma nostandard
int globalvalue NEWS__PROTOERR, NEWS__NOCONNECT, NEWS__SENDERR,
    	    	NEWS__RCVERR, NEWS__EOLIST;
#pragma standard

    extern unsigned short htons(unsigned short);
    extern void put_output(char *);
    extern unsigned int get_logical(char *, char *);
    extern int gethostname(char *, int);
    extern struct hostent *gethostbyname(char *);
    extern int socket(int, int, int);
    extern int connect(int, struct sockaddr *, int);
    extern int setsockopt(int, int, int, char *, int);
    extern int send(int, char *, int, int);
    extern int recv(int, char *, int, int);

#define SRV__ECHO 1
#define SRV__NOECHO 0

    static int s;
    static int do_echo = 0;
    static unsigned short echo_chan = 0;
    static int nosignal = 0;
    static unsigned int last_check[2], check_intvl[2];
    static $DESCRIPTOR(intvldsc, "0 00:03:00.00");

/*
**++
**  ROUTINE:	get_hostname
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Returns the local IP host name.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	get_hostname(char *name, int namesize)
**
**  name:   	ASCIZ_string, write only, by reference
**  namesize:	integer, read only, by value
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

int get_hostname(char *name, int namesize) {

    if (gethostname(name, namesize) == 0) return SS$_NORMAL;
    return 0;

} /* get_hostname */

/*
**++
**  ROUTINE:	server_connect
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Connects to the server.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	server_connect(char *nodename)
**
**  nodename:	ASCIZ_string, read only, by reference
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
int server_connect(char *nodename) {

    struct hostent *h;
    struct sockaddr_in sin;
    char tmp[1024];
    size_t  len;
    int	    bufsize = 32768;
#ifndef inet_addr
    extern int inet_addr(char *);
#endif

    memset(&sin, 0, sizeof(sin));
#ifdef TCPWARE_V30
    sin.sin_addr = inet_addr(nodename);
#else
    sin.sin_addr.s_addr = inet_addr(nodename);
#endif
    if (sin.sin_addr.s_addr == (u_long) -1) {
    	h = gethostbyname(nodename);
    	if (h == NULL) {
    	    lib$signal(NEWS__NOCONNECT, 2, strlen(nodename), nodename);
    	    exit(NEWS__NOCONNECT|STS$M_INHIB_MSG);
    	}
    	sin.sin_family = h->h_addrtype;
    	memcpy(&sin.sin_addr, h->h_addr, h->h_length);
    } else {
    	sin.sin_family = AF_INET;
    }
    sin.sin_port = htons(119);

    sys$bintim(&intvldsc, check_intvl);
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
    	lib$signal(NEWS__NOCONNECT, 2, strlen(nodename), nodename);
    	exit(NEWS__NOCONNECT|STS$M_INHIB_MSG);
    }

    if (connect(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
    	lib$signal(NEWS__NOCONNECT, 2, strlen(nodename), nodename);
    	exit(NEWS__NOCONNECT|STS$M_INHIB_MSG);
    }

/*  setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *) &bufsize, sizeof(bufsize));*/
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *) &bufsize, sizeof(bufsize));

    do_echo = $VMS_STATUS_SUCCESS(get_logical("NEWSRDR_SHOW_NNTP",tmp));
    if (do_echo) {
    	struct dsc$descriptor dsc;
    	if (!$VMS_STATUS_SUCCESS(get_logical("NEWSRDR_NNTP_OUTPUT",tmp))) {
    	    strcpy(tmp, "SYS$OUTPUT");
    	}
    	dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    	dsc.dsc$b_class = DSC$K_CLASS_S;
    	dsc.dsc$w_length = strlen(tmp);
    	dsc.dsc$a_pointer = tmp;
    	if (!$VMS_STATUS_SUCCESS(sys$assign(&dsc, &echo_chan, 0, 0, 0))) {
    	    echo_chan = 0;
    	}
    }

    sys$gettim(last_check);

    return SS$_NORMAL;

} /* server_connect */

/*
**++
**  ROUTINE:	server_disconnect
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Disconnects from the server.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	server_disconnect()
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
int server_disconnect() {

    return SS$_NORMAL;  /* won't actually disconnect until image rundown */

} /* server_disconnect */

/*
**++
**  ROUTINE:	server_send
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Sends a line to the server (with CRLF attached)
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	server_send(char *buf)
**
**  buf:    ASCIZ_string, read only, by reference
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
int server_send(char *buf) {

    static char obuf[16384];
    int len;

    if (do_echo) PUT_OUTPUT(buf);
    len = strlen(buf);
    if (len > 16382) {
    	if (send(s, buf, len, 0) < 0) {
    	    if (nosignal) return NEWS__SENDERR;
    	    lib$stop(NEWS__SENDERR, 0);
    	}
    	if (send(s, "\015\012", 2, 0) < 0) {
    	    if (nosignal) return NEWS__SENDERR;
    	    lib$stop(NEWS__SENDERR, 0);
    	}
    } else {
    	if (len > 0) memcpy(obuf, buf, len);
    	memcpy(obuf+len, "\015\012", 2);
    	if (send(s, obuf, len+2, 0) < 0) {
    	    if (nosignal) return NEWS__SENDERR;
    	    lib$stop(NEWS__SENDERR, 0);
    	}
    }
    return SS$_NORMAL;

} /* server_send */

/*
**++
**  ROUTINE:	server_get_line
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Gets a "line" (a string terminated with CRLF) from the
**  server (stripping the CRLF before returning the string to caller).
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	server_get_line(char *out, int outsize, int *outlen)
**
**  out:    	ASCIZ (or regular) string, write only, by reference
**  outsize:	integer, read only, by value
**  outlen: 	integer, write only, by reference
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
int server_get_line(char *out, int outsize, int *outlen) {

    static char buf[16384];
    static char *bufp;
    static int buflen = 0;
    char *eostr;
    char *outp=out;
    int status, copylen;

    while (1) {
    	if (buflen == 0) {
    	    buflen = recv(s, buf, sizeof(buf)-1, 0);
    	    if (buflen < 0) {
    	    	buflen = 0;
    	    	if (nosignal) return NEWS__RCVERR;
    	    	lib$stop(NEWS__RCVERR, 0);
    	    }
    	    buf[buflen] = '\0';
    	    bufp = buf;
    	    if (*bufp == '\012') {bufp++; buflen--;}
    	}

    	eostr = strchr(bufp, '\015');
    	if (eostr == NULL) eostr = strchr(bufp, '\012');
    	if (eostr != NULL) {
    	    copylen = eostr-bufp > outsize ? outsize : eostr-bufp;
    	    memcpy(outp, bufp, copylen);
    	    outp += copylen;
    	    outsize -= copylen;
    	    buflen -= (eostr-bufp) + 1;
    	    bufp = eostr + 1;
    	    if ((*eostr == '\015') && (*bufp == '\012')) {
    	    	bufp++;
    	    	buflen--;
    	    }
    	    break;
    	} else {
    	    copylen = buflen > outsize ? outsize : buflen;
    	    memcpy(outp, bufp, copylen);
    	    outp += copylen;
    	    outsize -= copylen;
    	    buflen = 0;
    	}
    }

    status = SS$_NORMAL;
    if (outp-out == 1 && *out == '.') {
    	*out = '\0';
    	return NEWS__EOLIST;
    }
    if (outlen) {
    	*outlen = outp-out;
    } else {
    	*outp = '\0';
    }

    return SS$_NORMAL;

}  /* server_get_line */

/*
**++
**  ROUTINE:	server_get_reply
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Gets a numeric reply code from the server, possibly along
**  with some accompanying text.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	server_get_reply(int echo, int *code, [char *out], [int outsize],
**  	    	    	    	[int *outlen])
**
**  echo:   	boolean, read only, by value
**  code:   	integer, write only, by reference
**  out:    	ASCIZ (or regular) string, write only by reference (optional)
**  outsize:	integer, read only, by value (optional)
**  outlen: 	integer, write only, by reference (optional)
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
int server_get_reply(int echo, int *code, char *out, int outsize, int *outlen) {

    char tmp[1024], *cp;
    int status, tmplen;

    while (1) {
    	status = server_get_line(tmp, sizeof(tmp)-1, &tmplen);
    	if (!$VMS_STATUS_SUCCESS(status)) return status;
    	if (tmplen <= 3 || tmp[3] != '-') break;
    }
    *(tmp+tmplen) = '\0';

    if (echo == SRV__ECHO) put_output(tmp);
    if (do_echo) PUT_OUTPUT(tmp);

    if (tmplen < 3) lib$stop(NEWS__PROTOERR, 0);
    status = lib$cvt_dtb(3, tmp, code);
    if (!$VMS_STATUS_SUCCESS(status)) lib$stop(NEWS__PROTOERR, 0);
    if (out != NULL) {   
    	if ((tmplen -= 4) < 0) tmplen = 0;
    	if (tmplen > outsize) tmplen = outsize;
    	if (tmplen > 0) memcpy(out, tmp+4, tmplen);
    	if (outlen) {
    	    *outlen = tmplen;
    	} else {
    	    *(out+tmplen) = '\0';
    	}
    }

/*
**  Update last check time since we know the connection's still OK
*/
    sys$gettim(last_check);

    return SS$_NORMAL;

} /* server_get_reply */

/*
**++
**  ROUTINE:	server_check
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Checks that we're still connected to the server.
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
int server_check() {

    unsigned int now[2], then[2], junk[2];
    char tmp[1024];
    unsigned int status;
    int reply_code, len;

    sys$gettim(now);
    lib$add_times(last_check, check_intvl, then);
    if ($VMS_STATUS_SUCCESS(lib$sub_times(then, now, junk))) return;

    nosignal = 1;
    status = server_send("HELP");
    if (!$VMS_STATUS_SUCCESS(status)) {
    	nosignal = 0;
    	return status;
    }
    status = server_get_reply(SRV__NOECHO, &reply_code, tmp, sizeof(tmp)-1, &len);
    if ($VMS_STATUS_SUCCESS(status)) {
    	while (1) {
    	    status = server_get_line(tmp, sizeof(tmp)-1, &len);
    	    if (status == NEWS__EOLIST) {
    	    	nosignal = 0;
    	    	return SS$_NORMAL;
    	    }
    	    if (!$VMS_STATUS_SUCCESS(status)) break;
    	}
    }
    nosignal = 0;
    return status;

} /* server_check */
