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
**  	21-NOV-1994 V1.2    Madison 	Update for NETLIB V2.0.
**--
*/
#include "newsrdr.h"
#include "netlib_dir:netlibdef.h"
#include <iodef.h>

#define PUT_OUTPUT(x) {\
    if (echo_chan != 0) sys$qiow(0, echo_chan, IO$_WRITEVBLK,\
    	0, 0, 0, x, strlen(x), 0, 0x8d010000, 0, 0);\
    else put_output(x);}

    static int do_echo = 0;
    static unsigned short echo_chan = 0;
    static void *ipctx = 0;
    static unsigned int last_check[2], check_intvl[2];
    static $DESCRIPTOR(intvldsc, "0 00:03:00.00");
    static TIME tmo;
    static int nosignal = 0;

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
    char tmp[256];
    int i;
    static unsigned short port = 119;
    static $DESCRIPTOR(longtime, "9999 23:59:59.99");

    status = netlib_socket(&ipctx);
    if (!OK(status)) lib$stop(NEWS__NOCONNECT, 2, strlen(nodename), nodename,
    	    	    	    	status);
    INIT_SDESC(ndsc, strlen(nodename), nodename);
    status = netlib_connect_by_name(&ipctx, &ndsc, &port);
    if (!OK(status)) {
    	netlib_close(&ipctx);
    	lib$stop(NEWS__NOCONNECT, 2, strlen(nodename), nodename, status);
    }
    sys$bintim(&longtime, &tmo);
    sys$bintim(&intvldsc, &check_intvl);

    sys$gettim(last_check);
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

    if (ipctx != 0) {
    	netlib_close(&ipctx);
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

    if (do_echo) PUT_OUTPUT(str);
    INIT_SDESC(sdsc, strlen(str), str);
    status = netlib_writeline(&ipctx, &sdsc);
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
    unsigned short retlen;

    INIT_SDESC(dsc, outsize, out);
    status = netlib_readline(&ipctx, &dsc, &retlen, 0, &tmo);
    if (!OK(status)) {
    	if (nosignal) return NEWS__RCVERR;
    	server_disconnect();
    	lib$stop(NEWS__RCVERR, 0, status);
    }
    if (retlen == 1 && *out == '.') {
    	*out = '\0';
    	if (outlen) *outlen = 0;
    	return NEWS__EOLIST;
    }

    if (retlen > 0 && *out == '.') {
    	retlen -= 1;
    	memmove(out, out+1, retlen);
    }

    if (outlen) {
    	*outlen = retlen;
    } else {
    	*(out+retlen) = '\0';
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
    char tmp[1024];
    unsigned int status;
    unsigned short retlen;
    int len;

    INIT_SDESC(dsc, sizeof(tmp)-1, tmp);
    while (1) {
    	status = netlib_readline(&ipctx, &dsc, &retlen, 0, &tmo);
    	if (retlen <= 3 || *(dsc.dsc$a_pointer+3) != '-') break;
    }
    tmp[retlen] = '\0';
    dsc.dsc$w_length = retlen;
    if (echo == SRV__ECHO) put_output_dx(&dsc);
    if (do_echo) PUT_OUTPUT(tmp);
    if (!OK(status) || retlen < 3) {
    	server_disconnect();
    	lib$stop(NEWS__PROTOERR, 0, status);
    }
    status = lib$cvt_dtb(3, dsc.dsc$a_pointer, code);
    if (!OK(status)) {
    	server_disconnect();
    	lib$stop(NEWS__PROTOERR, 0, status);
    }
    if (out) {
    	len = retlen-5 < outsize-1 ? retlen-5 : outsize-1;
    	if (len < 0) len = 0;
    	if (len > 0) memcpy(out, tmp+4, len);
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
    unsigned short retlen;

    INIT_SDESC(dsc, hostname_size-1, hostname);
    status = netlib_get_hostname(&dsc, &retlen);
    if (OK(status)) hostname[retlen] = '\0';

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
