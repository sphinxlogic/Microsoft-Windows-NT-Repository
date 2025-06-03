/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Server communication routines for use with NETLIB.
**
**  MODULE DESCRIPTION:
**
**  	This module contains the server_xxx routines used by NEWSRDR,
**  written for use with the NETLIB library of access routines.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  12-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	12-SEP-1992 V1.0    Madison 	Initial coding.
**  	17-FEB-1993 V1.0-1  Madison 	Update for new message formats.
**  	23-APR-1993 V1.1    Madison 	New server_check routine.
**  	03-JUN-1993 V1.1-1  Madison 	Got time check backwards!
**--
*/
#include "newsrdr.h"

    static unsigned int ipctx = 0;
    static unsigned int last_check[2], check_intvl[2];
    static $DESCRIPTOR(intvldsc, "0 00:03:00.00");
    static TIME tmo;
    static int nosignal = 0;

#define NET_K_TCP 1
#define NET_K_UDP 2
#define NET_M_PUSH 1
#define NET_M_NOTRM 2

    extern unsigned int net_assign(), net_bind(), tcp_connect();
    extern unsigned int tcp_disconnect(), tcp_send(), tcp_get_line();
    extern unsigned int net_deassign(), net_get_hostname();

/*
**++
**  ROUTINE:	server_connect
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Connect to the server.
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
unsigned int server_connect(char *nodename) {

    struct dsc$descriptor ndsc;
    unsigned int status;
    static $DESCRIPTOR(longtime, "9999 23:59:59.99");

    status = net_assign(&ipctx);
    if (!OK(status)) lib$stop(NEWS__NOCONNECT, 2, strlen(nodename), nodename,
    	    	    	    	status);
    status = net_bind(&ipctx, NET_K_TCP);
    if (!OK(status)) {
    	net_deassign(&ipctx);
    	lib$stop(NEWS__NOCONNECT, 2, strlen(nodename), nodename, status);
    }
    INIT_SDESC(ndsc, strlen(nodename), nodename);
    status = tcp_connect(&ipctx, &ndsc, 119);
    if (!OK(status)) {
    	net_deassign(&ipctx);
    	lib$stop(NEWS__NOCONNECT, 2, strlen(nodename), nodename, status);
    }
    sys$bintim(&longtime, &tmo);

    sys$gettim(last_check);

    return SS$_NORMAL;

} /* server_connect */

/*
**++
**  ROUTINE:	server_disconnect
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Disconnect from the server.
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
unsigned int server_disconnect() {

    if (ipctx) {
    	tcp_disconnect(&ipctx);
    	net_deassign(&ipctx);
    	ipctx = 0;
    }

    return SS$_NORMAL;

} /* server_disconnect */

/*
**++
**  ROUTINE:	server_send
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Send a line to the server.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	server_send(char *str)
**
**  str:    ASCIZ_string, read only, by reference
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
unsigned int server_send(char *str) {

    struct dsc$descriptor sdsc;
    unsigned int status;

    INIT_SDESC(sdsc, strlen(str), str);
    status = tcp_send(&ipctx, &sdsc, NET_M_PUSH);
    if (!OK(status)) {
    	if (nosignal) return NEWS__SENDERR;
    	server_disconnect();
    	lib$stop(NEWS__SENDERR, 0, status);
    }

    return SS$_NORMAL;

} /* server_send */

/*
**++
**  ROUTINE:	server_get_line
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Gets a "line"... that is, a string terminated by <CRLF>,
**  from the server (stripping off the CRLF before returning to caller).
**  If outlen is specified, no null terminator is added to out.  Otherwise,
**  a null byte is added.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	server_get_line(char *out, int outsize, [int *outlen])
**
**  out:    	ASCIZ or just plain char_string, write only, by reference
**  outsize:	integer, read only, by value
**  outlen: 	integer, write only, by refrence (optional)
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
unsigned int server_get_line(char *out, int outsize, int *outlen) {

    struct dsc$descriptor dsc;
    unsigned int status;
    int len;

    INIT_DYNDESC(dsc);
    status = tcp_get_line(&ipctx, &dsc, 0, 0, 0, &tmo);
    if (!OK(status)) {
    	if (nosignal) return NEWS__RCVERR;
    	server_disconnect();
    	lib$stop(NEWS__RCVERR, 0, status);
    }
    if (dsc.dsc$w_length == 1 && *dsc.dsc$a_pointer == '.') {
    	str$free1_dx(&dsc);
    	*out = '\0';
    	if (outlen) *outlen = 0;
    	return NEWS__EOLIST;
    } 

    len = (dsc.dsc$w_length < outsize-1) ? dsc.dsc$w_length : outsize-1;
    memcpy(out, dsc.dsc$a_pointer, len);
    str$free1_dx(&dsc);
    if (outlen) {
    	*outlen = len;
    } else {
    	*(out+len) = '\0';
    }

    return SS$_NORMAL;

} /* server_get_line */

/*
**++
**  ROUTINE:	server_get_reply
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Gets a numeric status code-type reply from the server, possibly
**  along with the accompanying text.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	server_get_reply(int echo, int *code, [char *out], [int outsize],
**  	    	    	    	[int *outlen])
**
**  echo:   	boolean, read only, by value
**  out:    	ASCIZ (or regular) string, write only, by reference (optional)
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
unsigned int server_get_reply(int echo, int *code, char *out, int outsize,
    	    	    	    	    	int *outlen) {
    struct dsc$descriptor dsc;
    unsigned int status;
    int len;

    INIT_DYNDESC(dsc);
    while (1) {
    	status = tcp_get_line(&ipctx, &dsc, 0, 0, 0, &tmo);
    	if (dsc.dsc$w_length <= 3 || *(dsc.dsc$a_pointer+3) != '-') break;
    }
    if (echo == SRV__ECHO) put_output_dx(&dsc);
    if (!OK(status) || dsc.dsc$w_length < 3) {
    	server_disconnect();
    	lib$stop(NEWS__PROTOERR, 0, status);
    }
    status = lib$cvt_dtb(3, dsc.dsc$a_pointer, code);
    if (!OK(status)) {
    	server_disconnect();
    	lib$stop(NEWS__PROTOERR, 0, status);
    }
    if (out) {
    	len = dsc.dsc$w_length-5 < outsize-1 ? dsc.dsc$w_length-5 : outsize-1;
    	if (len < 0) len = 0;
    	if (len > 0) memcpy(out, dsc.dsc$a_pointer+4, len);
    	str$free1_dx(&dsc);
    	if (outlen) {
    	    *outlen = len;
    	} else {
    	    *(out+len) = '\0';
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
**  	get_hostname(char *hostname, int hostname_size)
**
**  hostname:	    ASCIZ_string, write only, by reference
**  hostname_size:  integer, read only, by value
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
unsigned int get_hostname(char *hostname, int hostname_size) {

    struct dsc$descriptor dsc;
    unsigned int status;
    int len;

    INIT_DYNDESC(dsc);
    status = net_get_hostname(&dsc);
    if (OK(status)) {
    	len = dsc.dsc$w_length < hostname_size-1 ?
    	    	    	dsc.dsc$w_length : hostname_size-1;
    	memcpy(hostname, dsc.dsc$a_pointer, len);
    	*(hostname+len) = '\0';
    	str$free1_dx(&dsc);
    }

    return status;

} /* get_hostname */

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
unsigned int server_check() {

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
