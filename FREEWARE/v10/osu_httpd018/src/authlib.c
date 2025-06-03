/* This module provides a support routine to writing authenticator programs
 * for the DECthreads HTTP server.  When the server gets a request for a 
 *  protected file, it uses the authenticator program to determine whether the 
 * access is to be permitted.  The server serializes the requests so that
 * the authenticator will process them sequentially and not have to do any
 * multi-threading.
 *
 * The authenticator program initializes it's own data structures and
 * calls the following function to start receiving authentication requests:
 *
 * 	auth_server ( int argc, char **argv, char *log, code_count, 
 *		int authenticate(), ... );
 *
 * This routine, which only returns on error (e.g. HTTP server dies), takes
 * for following arguments:
 *
 *	argc, argv 	Argument vector that was passed to main() routine
 *			by the image activator and therefore contains the
 *			command line arguments used to start the program.
 *			The HTTP server passes the names of the communication
 *			mailboxes on the command line when it spawns the
 *			authenticator.
 *
 *	log		Status line to be logged in HTTP server's log file,
 *			usually announcing the name and version of the
 *			authenicator program (e.q. "Farquar Authenticator, 
 *			version 1.0,...").
 *
 *	code_count	Number of callback routines.  Each callback routine
 *			is for a different message code, 1 through code_count.
 *			A request for a code for which there is no callback
 *			will response with success.
 *
 *	authenticate	A caller-supplied authentication callback routine.
 *			See below for a descriptions of its arguments.
 *
 * The authenticator supplies the the auth_server() routine with a callback
 * function that is called to process the authentication requests.  It
 * is called with the following format:
 *
 *	int status = authenticate ( int local_port, int remote_port,
 *			unsigned char *remote_address, char *method,
 *			char *setup_file, char *ident, char *authorization,
 *			char **response, char **log );
 *
 * The authenticate routines arguments:
 *	status		Boolean flag indicating whether access is to
 *			be granted (1) or denied (0).  Additional detail
 *			concerning the nature of a failure (error, protection
 *			violation) must be returned via the response and log
 *			arguments.
 *
 *	local_port	Local port number that recieved the request (80).
 *
 *	remote_port	Remote port number of client connection that made
 *			the request.
 *
 *	remote_address	Internet address of remote client, in network data order.
 *
 *	method		Method field extracted from client request (i.e. "GET",
 *			"PUT", etc), truncated to seven character plus a 
 *			terminating null.
 *
 *	setup_file	Name of protection setup file to use to validate
 *			request.  This file is extracted from the configuration
 *			rules file protect rule that caused the authentication
 *			to take place.  The setup file typically contains
 *			pointers to additional databases used to validate
 *			the request: password file, group file.
 *
 *	ident		Translated identifier (file pathname) that the
 *			client wishes to access.
 *
 *	authorization	Authorization lines extracted from the client
 *			request headers.  Each line is separated by a linefeed
 *			character and includes the header label.
 *
 *	response	Address of a pointer to receive an HTTP response
 *			header when authentication fails.  This consists
 *			of a status line with numeric failure code followed
 *			by "WWW-authenticate:" headers indicating the
 *			schemes supported.
 *
 *	log		Text to be added to HTTP server's log file.  The
 *			server may or may not add the log data to the file,
 *			depending upon the level of logging enabled.
 *
 * Other routines defined by authlib.c:
 *    int auth_basic_authorization ( char *authorization_headers,
 *		char *user, int ulen, char *password, int plen, int upcase )
 *
 *    int auth_pubkey_authorization ( char *authorization_headers,
 *		char *user, int ulen, char *password, int plen, int upcase )
 *
 *    int auth_main_loop ( int req_chan, int rsp_chan, char *log, 
 *		int authenticate());
 *
 * Command line:
 *	$ http_authenticator request_mailbox response_mailbox
 *
 * Arguments:
 *	request_mailbox   Logical name assigned to the mailbox that server
 *			will write requests to.  The authenticator reads this
 *			mailbox.
 *
 *	response_mailbox  Logical name assigned to mailbox that authenticator
 *			write responses to.  The HTTP server reads this
 *			mailbox.
 *
 * Author:	David Jones
 * Date:	29-MAY-1994
 * Revised:	31-AUG-1994		Exit more gracefully on SS$_NOWRITER,
 *					routine auth_main_loop.
 * Revised:	23-FEB-1995		Use auth_callback typedef
 */
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <descrip.h>
#include <string.h>
#include <iodef.h>
#ifndef IO$M_READERCHECK
/*
 * Out-of-date iodef.h, hard code the values for new mailbox driver options.
 */
#define IO$M_READERCHECK 0x0100
#define IO$M_WRITERCHECK 0x0200
#define CMB$M_READONLY 1
#define CMB$M_WRITEONLY 2
#else
#include <cmbdef.h>
#endif
#include <ssdef.h>
/* The ssdef.h is out of date, hard code new values */
#ifndef SS$_NOREADER
#define SS$_NOREADER 0x024c4
#define SS$_NOWRITER 0x024cc
#endif

#include "authlib.h"
#include "authcom.h"
static int decode_char();	/* fwd references */
/*
 * Define prototypes for exported/imported functions in authlib.c.
 */
int auth_server ( int argc, char **argv, char *log_line, int code_count, ... );

auth_callback auth_unimplemented;
/**************************************************************************/
int auth_unimplemented ( int lport, int rport, unsigned char *raddr,
	char *method, char *setup, char *ident, char *auth,
	char **sts_line, char **log_line ) {
    return 1;
printf("Unimplemented function called\n");
}
/*****************************************************************************/
/* Main routine, establish communication with server and enter loop
 * that reads requests, processes them, and sends response.
 */
int auth_server ( int argc, char **argv, char *log_line, int code_count, ... )
{
    va_list callback;
    int status, SYS$CREMBX(), SYS$SETPRN(), req_mbx, rsp_mbx, auth_main_loop();
    int i;
    auth_callback **cb_list;
    char reqnam[40], rspnam[40];
    $DESCRIPTOR(req_mbx_name,"");
    $DESCRIPTOR(rsp_mbx_name,"");
    /*
     * Count the command line arguments.
     */
    if ( argc < 3 ) {
	fprintf ( stderr, 
	    "Missing argument(s), usage: authenticator reqmbx rspmbx\n" );
	return 20;
    }
    /*
     * Create mailboxes.   Make them directional so we can detect the
     * http_server dying on us.
     */
    req_mbx = rsp_mbx = 0;
    req_mbx_name.dsc$w_length = strlen ( argv[1] );
    req_mbx_name.dsc$a_pointer = reqnam;
    if ( req_mbx_name.dsc$w_length > 39 ) req_mbx_name.dsc$w_length = 39;
    for ( i=0; i < req_mbx_name.dsc$w_length; i++ ) 
		reqnam[i] = toupper(argv[1][i]);

    status = SYS$CREMBX ( 0, &req_mbx, sizeof(struct msg1), 
	sizeof(struct msg1),MBX_PROT, 0, &req_mbx_name, CMB$M_READONLY );
    if ( (status&1) == 0 ) return status;

    rsp_mbx_name.dsc$w_length = strlen ( argv[2] );
    rsp_mbx_name.dsc$a_pointer = rspnam;
    if ( rsp_mbx_name.dsc$w_length > 39 ) rsp_mbx_name.dsc$w_length = 39;
    for ( i=0; i < rsp_mbx_name.dsc$w_length; i++ ) 
		rspnam[i] = toupper(argv[2][i]);

    status = SYS$CREMBX ( 0, &rsp_mbx, sizeof(struct msg2), 
	sizeof(struct msg2), MBX_PROT, 0, &rsp_mbx_name, CMB$M_WRITEONLY );
    if ( (status&1) == 0 ) return status;
    /*
     * Initialize data structures used to process requests.  Allocate
     * array of callback routines.
     */
    va_start ( callback, code_count );
    cb_list = malloc ( sizeof(auth_callback *) * (code_count+1) );
    cb_list[0] = auth_unimplemented;
    for ( i = 1; i <= code_count; i++ ) {
	cb_list[i] = *((auth_callback **) callback);
	va_arg(callback, auth_callback *);
    }
    /*
     * Send status line back to server to signal it that we are ready.
     * The text data sent will be placed in the log file, so include
     * version information.  The main loop function only resturns on  an error.
     */
    status = auth_main_loop ( req_mbx, rsp_mbx, log_line, code_count, cb_list );
    return status;
}
/******************************************************************************/
/* Main loop for processing requests from server.
 */
int auth_main_loop ( int req_mbx, int rsp_mbx, char *init_status, 
	int code_count, auth_callback **cb_list )
{
    int ef, status, SYS$QIOW(), LIB$GET_EF(), length, code;
    char *sts_line, *log_line;
    struct msg1 req_msg;
    struct msg2 rsp_msg;
    struct { unsigned short status, count; long pid; } iosb;
    char setup_file[256], ident[256], authorization[1000];
    /*
     * First construct first response message and send to server, the
     * server is stalled waiting for us to signal it to proceed via this 
     * message.
     */
    LIB$GET_EF ( &ef );
    rsp_msg.status = 1;
    rsp_msg.sts_len = 0;
    rsp_msg.log_len = strlen(init_status);
    strcpy ( rsp_msg.data, init_status );
    length = RSP_HDR_SIZE + rsp_msg.log_len;

    status = SYS$QIOW ( ef, rsp_mbx, IO$_WRITEVBLK|IO$M_READERCHECK, &iosb,
	0, 0, &rsp_msg, length, 0, 0, 0, 0 );
    if ( (status&1) == 1 ) status = iosb.status;
    /*
     * Now loop for requests.
     */
    while ( (status&1) == 1 ) {
	/*
	 * Read request.
	 */
        status = SYS$QIOW ( ef, req_mbx, IO$_READVBLK|IO$M_WRITERCHECK, &iosb,
	    0, 0, &req_msg, sizeof(req_msg), 0, 0, 0, 0 );
        if ( (status&1) == 1 ) status = iosb.status;
        if ( (status&1) == 0 ) break;
        /*
         * Breakout request arguments.
	 */
	length = req_msg.setup_len;
	if ( length > sizeof(setup_file)-1 ) length = sizeof(setup_file)-1;
	strncpy ( setup_file, req_msg.data, length );
	setup_file[length] = '\0';

	length = req_msg.ident_len;
	if ( length > sizeof(ident)-1 ) length = sizeof(ident)-1;
	strncpy ( ident, &req_msg.data[req_msg.setup_len], length );
	ident[length] = '\0';

	length = iosb.count - REQ_HDR_SIZE - req_msg.setup_len -
		req_msg.ident_len;
	if ( length < 0 ) length = 0;
	strncpy ( authorization, 
	    &req_msg.data[req_msg.setup_len+req_msg.ident_len], length );
	authorization[length] = '\0';
	/*
	 * Perform access check and build response message.
	 */
	sts_line = "";
	log_line = "";
	code = req_msg.code;
	if ( (code < 1) || (code > code_count) ) code = 0;

	rsp_msg.status = (cb_list[code]) ( req_msg.local_port, 
		req_msg.remote_port,
		req_msg.remote_addr, req_msg.method,
		setup_file, ident, authorization, &sts_line, &log_line );
	rsp_msg.sts_len = strlen ( sts_line );
	strcpy ( rsp_msg.data, sts_line );
	rsp_msg.log_len = strlen ( log_line );
	strcpy ( &rsp_msg.data[rsp_msg.sts_len], log_line );
    	length = rsp_msg.sts_len + rsp_msg.log_len + RSP_HDR_SIZE;
	
	/*
	 * Write response.
	 */
        status = SYS$QIOW ( ef, rsp_mbx, IO$_WRITEVBLK|IO$M_READERCHECK, &iosb,
		0, 0, &rsp_msg, length, 0, 0, 0, 0 );
    	if ( (status&1) == 1 ) status = iosb.status;
    }
    /*
     * Convert 'No writer' status to normal.
     */
    if ( status == SS$_NOWRITER ) status = 1;
    return status;
}
/******************************************************************************/
/* Utility routine for parsing basic scheme authorization info from
 * authorization lines.  Return value is 1 if data extracted, 0 if not found.
 * username and password are truncated to 
 */
int auth_basic_authorization ( char *astring,	/* auth. header lines */
	char *user, int ulen, char *password, int plen, int upcase )
{
    int i, j, status, ctx;
    char *line, *label, *encoded, c;
    /*
     * First scan lines for the first Authorization:.
     */
    label = "authorization";
    line = astring;
    encoded = "";
    for ( i = 0; astring[i]; i++ ) if ( astring[i] == '\n' ) {
	for ( j = 0; (line[j] != ':') && (line[j] != '\n'); j++ ) {
	    c = _tolower(line[j]);
	    if ( c != label[j] ) break;
	}
	if ( line[j] != ':' ) continue;
	/*
	 * found authorization line, see if scheme is basic.
	 */
	for ( j++; line[j] != '\n' && isspace(line[j]); j++ );	/* skip white*/
fprintf(stderr,"/authlib/ auth. scheme found: %s\n", &line[j] );

	for ( label="basic"; !isspace(line[j]) && line[j] != '\n'; j++ ) {
		c = _tolower(line[j]);
		if ( c != *label++ ) break;
	}
	if ( !isspace(line[j]) ) continue;	/* Not basic */
	/*
	 * rest of line is encoded.
	 */
	for ( j++; line[j] != '\n' && isspace(line[j]); j++ );	/* skip white*/
	encoded = &line[j];
    }
    if ( *encoded == '\0' ) return 0;
    /*
     * Decode the encoded characters up to the line feed.
     */
    ctx = 0;
    c = '\0';
    while ( decode_char(&encoded, &ctx, &c) ) {
	if ( c == ':' ) break;
	if ( upcase ) c = _toupper(c);
	if ( ulen > 1 ) { *user++ = c; ulen--; };
    }
    *user = '\0';
    if ( c == ':' ) {
	while ( decode_char ( &encoded, &ctx, &c ) ) {
	    if ( upcase ) c = _toupper(c);
	    if ( plen > 1 ) { *password++ = c; plen--; };
	}
    }
    *password = '\0';
    return 1;
}

/*
 * Decode block of 4 characters to 1,2 or 3 characters.  Return 0 if
 * block is invalid.
 */
static int decode_char ( char **inp, int *context, unsigned char *out )
{
    char *enc_table = 
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
   int context_bits, context_count, i, j;
   char *p, c;
   context_bits = *context;
   context_count = (context_bits>>12);
   context_bits = context_bits&4095;
   
   p = *inp;
   while ( context_count < 8 ) {
       c = *p++;
	if ( c == '=' ) return 0;
	if ( c == '\0' ) return 0;
       for ( j = 0; (enc_table[j] != c); j++ ) if ( !enc_table[j] ) return 0;
	context_bits = (context_bits&63)<<6 | j;
	context_count += 6;
   }
    if ( context_count == 8 ) *out = (context_bits&255);
    else *out = (context_bits>>(context_count-8))&255;
   *context = (context_bits&4095) | ((context_count-8)<<12);
   *inp = p;
   return 1;
}
/******************************************************************************/
/* Utility routine for parsing basic scheme authorization info from
 * authorization lines.  Return value is 1 if data extracted, 0 if not found.
 * username and password are truncated to 
 */
int auth_pubkey_authorization ( char *astring,	char *privkey,
	char *user, int ulen, char *password, int plen, int upcase )
{
    int i, j, status, ctx;
    char *line, *label, *encoded, c;
    /*
     * First scan lines for the first Authorization:.
     */
    label = "authorization";
    line = astring;
    encoded = "";
    for ( i = 0; astring[i]; i++ ) if ( astring[i] == '\n' ) {
	for ( j = 0; (line[j] != ':') && (line[j] != '\n'); j++ ) {
	    c = _tolower(line[j]);
	    if ( c != label[j] ) break;
	}
	if ( line[j] != ':' ) continue;
	/*
	 * found authorization line, see if scheme is pubkey.
	 */
fprintf(stderr,"/authlib/ auth. sheme found: %s\n", label );
	for ( j++; line[j] != '\n' && isspace(line[j]); j++ );	/* skip white*/
	for ( label="pubkey"; !isspace(line[j]) && line[j] != '\n'; j++ ) {
		c = _tolower(line[j]);
		if ( c != *label++ ) break;
	}
	if ( !isspace(line[j]) ) continue;	/* Not basic */
	/*
	 * rest of line is encoded.
	 */
	for ( j++; line[j] != '\n' && isspace(line[j]); j++ );	/* skip white*/
	encoded = &line[j];
    }
    if ( *encoded == '\0' ) return 0;
    /*
     * Decode the encoded characters up to the line feed.
     */
    ctx = 0;
    c = '\0';
    while ( decode_char(&encoded, &ctx, (unsigned char *) &c) ) {
	if ( c == ':' ) break;
	if ( upcase ) c = _toupper(c);
	if ( ulen > 1 ) { *user++ = c; ulen--; };
    }
    *user = '\0';
    if ( c == ':' ) {
	while ( decode_char ( &encoded, &ctx, (unsigned char *) &c ) ) {
	    if ( upcase ) c = _toupper(c);
	    if ( plen > 1 ) { *password++ = c; plen--; };
	}
    }
    *password = '\0';
    return 1;
}
