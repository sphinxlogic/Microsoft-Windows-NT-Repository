/*
 * The script_execute routine handles the interaction of the HTTP server with
 * the WWWEXEC decnet object.
 *
 * int http_script_execute ( session_ctx cnx, char *subfunct,
 *		char *ident, char *arg, string *iobuf );
 *
 * Revised:  6-APR-1994		Add support for exec directives and <DNETPATH>
 * Revised:  5-MAY-1994		Support node:: prefix on bin directory
 *				(Sugessted by Robin Garner, torobin@svh.unsw.edu.au)
 * Revised:   28-MAY-1994	Added INVCACHE option and fixed bug in
 *				tag_list processing.
 * reviesd:   28-JUN-1994	Fixed operation of CGI mode not hang when
 *				script sends invalid response (</DNETCGI>
 *				was not being checked for when reading header.
 * Revised:    7-JUL-1994	Add access check.
 * Revised:	1-AUG-1994	Support alternate object for WWWEXEC.
 * Revised:     7-AUG-1994	Fix problem with DNETRAW being treated as
 *				producing no output.
 * Revised:	11-AUG-1994	Fix DNETRAW so first data message goes into
 *				response header buffer for logging.
 * Revised:	1-SEP-1994	Fix bad maxlen in tu_read_line call.
 * Revised:	10-SEP-1994	Include version string.
 * Revised:	15-SEP-1994	Add more error checks to CGI processing.
 * Revised:	10-OCT-1994	Add <DNETRECMODE> tag.
 * Revised:     18-OCT-1994	Add <DNETID2> tag.
 * Revised:     17-NOV-1994	Fixed bug in flushing of linefeed following
 *				CR in CGI mode.
 * Revised:	4-JAN-1995	pass along unrecognized headers to client
 *				when in CGI mode.
 * Revised:     19-JAN-1995	Correct calculation of byte count on
 *				stream buffer flush.
 */
#include "pthread_np.h"
#include <stdio.h>
#include <stdlib.h>
#include "session.h"
#include "tserver_tcp.h"
#include "decnet_access.h"
#include "decnet_searchlist.h"
#define VMS_EOF 2162

int http_log_level;			/* global variable */
extern char http_server_version[];	/* Global variable, server version */
int tlog_putlog();
char *http_default_host;
int http_dns_enable;		/* GLobal variable, name lookup enable */
int http_send_error 		/* prototype for send_error function */
	( session_ctx scb, char *msg, char *buf );
int http_add_response(), http_send_response_header(), http_translate_ident();
int http_invalidate_doc_cache(), http_check_protection();

static pthread_once_t wwwexec_setup = pthread_once_init;
static char *www_exec_task;
static int www_exec_node;		/* Length of node name portion */
static enum opcodes { DNET_HDR, DNET_ARG, DNET_ARG2, DNET_INPUT,
	DNET_TEXT, DNET_RAW, DNET_RQURL, DNET_CGI, DNET_HOST, DNET_ID,
	DNET_BINDIR, DNET_PATH, DNET_XLATE, DNET_SENTINEL, DNET_INVCACHE,
	DNET_RECMODE, DNET_ID2 };

static struct { enum opcodes opc; 	/* opcode */
		int l; 			/* Length of tag name (s) */
		char *s; 		/* Tag name */
		int terminal; } 	/* If true, ends session */
    tag_list[] = {
	{ DNET_HDR, 9, "<DNETHDR>", 0 }, 	/* Send header */
	{ DNET_ARG, 9, "<DNETARG>", 0 }, 	/* Send search arg */
	{ DNET_ARG2, 10, "<DNETARG2>", 0 },	/* Send trunc. search arg */
	{ DNET_INPUT, 11, "<DNETINPUT>", 0 },	/* Send client data */
	{ DNET_TEXT, 10, "<DNETTEXT>", 1 }, 	/* Read text response */
	{ DNET_RAW, 9, "<DNETRAW>", 1 }, 	/* Read HTTP response */
	{ DNET_RQURL, 11, "<DNETRQURL>", 0 },	/* Send original URL */
	{ DNET_CGI, 9, "<DNETCGI>", 1 }, 	/* Read 'CGI' response */
	{ DNET_HOST, 10,"<DNETHOST>", 0 },	/* Send server host */
	{ DNET_ID, 8,"<DNETID>", 0 },		/* Send connection info */
	{ DNET_BINDIR, 12,"<DNETBINDIR>", 0 },	/* Send htbin/exec directory */
	{ DNET_PATH, 10, "<DNETPATH>", 0 },	/* Send htbin/exec prefix */
        { DNET_XLATE, 11, "<DNETXLATE>", 0 },	/* Translate URL by rules file*/
	{ DNET_INVCACHE, 14, "<DNETINVCACHE>", 0 },
        { DNET_RECMODE, 13, "<DNETRECMODE>", 0 },
	{ DNET_ID2, 9, "<DNETID2>", 0 },	/* extended ID2 */
	{ DNET_SENTINEL, -1, "Sentinel", 1 }
    };

static int http_decnet_cgi 		/* Forward fucking reference */
	( session_ctx scb, 		/* Session control block */
	  void *dptr, 			/* Decnet connection to script task */
	  int text_mode,
	  string *iobuf );		/* Scratch data buffer for I/O */

/****************************************************************************/
static void www_init()
{
    dnet_initialize();
}

/****************************************************************************/
/*
 * Common routine to relay DECnet task output to client.
 */
static int transfer_output ( int text_mode, void *dptr, void *ctx, 
	string *iobuf,
	char * end_tag, int *data_bytes )
{
    char *buffer;
    int length, status, tag_length, iosize;
    tag_length = tu_strlen(end_tag);
    buffer = iobuf->s;
    iosize = iobuf->l > 4096 ? 4096 : iobuf->l;
    while ( ((status=dnet_read(dptr,buffer,iosize,&length))&1) == 1 ) {
	if ( (length == tag_length) && ( 0 ==
			tu_strncmp(buffer,end_tag, tag_length)) ) break;
	if ( text_mode ) {
	    /* Add CRLF if in text mode */
	    buffer[length++] = '\r'; buffer[length++] = '\n';
	}
	status = ts_tcp_write ( ctx, buffer, length );
	if ( (status&1) == 0 ) break;
	*data_bytes += length;
    }
    return status;
}
/**************************************************************************/
int http_script_execute (
	session_ctx scb,		/* Session control block */
	char *subfunc, 			/* Subfunction to execute */
	char *ident,			/* Ident parsed from request. */
	char *arg,			/* Search argument portion of URL */
	string *iobuf )			/* I/O buffer */
{
    int status, i, j, written, length, opcode, bufsize, id_len, prefix_len;
    int rr_pos, first, text_mode;
    int  dir_len, local_port, remote_port, remote_addr, siglen, terminal;
    struct acc_info_struct acc;
    char *buffer, *errmsg, *tmp, *bindir, **exec_list;
    void *dptr;				/* Handle for DECnet connection */
    /*
     * Check access protection.
     */
    pthread_once ( &wwwexec_setup, www_init );
    if ( scb->acc->prot_file[0] ) {
        status = http_check_protection  ( scb, ident, iobuf );
        if ( status == 0 ) {
	    /*
	     * Send error response to client, since this type of failure
	     * is 'normal', don't  echo request headers in response.
	     */
	    status = http_send_response_header ( scb->cnx, scb->rsphdr );
	    if ( (status&1) == 1 ) status = ts_tcp_write ( scb->cnx,
		"File _protected_ against access\r\n", 33 );
	    return status;
	}
    }
    /*
     * Parse ident portion into ident, dir-template, and bindir.
     * The rule file database is kludged up to store all 3 separated by
     * asteriks.
     */
    dir_len = id_len = -1;
    bindir = "";
    for ( i = 0; ident[i]; i++ ) if ( ident[i] == '*' ) {
	if ( id_len == -1 ) id_len = i;	/* first asterik */
	else if ( dir_len == -1 ) {		/* Second asterik */
	    dir_len = i - id_len - 1;
	    bindir = &ident[i+1];		/* Script directory */
	}
    }
    if ( id_len < 0 ) id_len = i;	/* fallback length */
    if ( dir_len < 0 ) dir_len = 0;
    if ( http_log_level > 9 ) tlog_putlog( 9,
	"dnet exec Ident: !AZ parse lengths: !SL !SL, dir: !AZ!/", 
	ident, id_len, dir_len, bindir );
    /*
     * Generate DECnet task specification to  use.
     */
    status = dnetx_parse_task ( bindir, &prefix_len, &exec_list, &rr_pos );
    if ( (status&1) == 0 ) {
	status = http_send_error ( scb,
		"500 Error connecting to DECnet object",
		"Unable to parse script configuration." );
	return status;
    }
    bindir = &bindir[prefix_len];	/* trim prefix */
    /*
     * Make connection to wwwexec object.  Make repeated attemtps to
     * Get successful connection, starting with task_specification at
     * Round Roibin pointer returned by parse function.
     */
    bufsize = iobuf->l;
    buffer = iobuf->s;

    for ( j = rr_pos, first=1; first || (j != rr_pos);
		j = exec_list[j+1] ? j+1 : 0 ) {
	first = 0;
        status = dnet_connect ( exec_list[j], &dptr );
        if ( http_log_level > 5 ) tlog_putlog ( 5,
		"Connect status=!SL for task_spec[!SL] = '!AZ'!/", status, j,
		exec_list[j] );
        if ( (status&1) == 0 ) {
	    dnet_format_error ( status, buffer, bufsize > 256 ? 256 : bufsize );
	    errmsg = "500 Error connecting to DECnet object";
	    continue;
        }
        /*
         * Send header information, 4 messages.
         */
        status = dnet_write ( dptr, subfunc, tu_strlen(subfunc), &written );
        for ( i = 0; i < 3; i++ ) if ( (status&1) == 1 ) switch ( i ) {
          case 0:	/* Method */
            status = dnet_write ( dptr, scb->request[0].s, 
		scb->request[0].l > 255 ? 255 : scb->request[0].l, &written );
	    break;
          case 1:	/* protocol */
            status = dnet_write ( dptr, scb->request[2].s, 
		scb->request[2].l > 255 ? 255 : scb->request[2].l, &written );
	    break;
          case 2:	/* ident portion of URL */
	    length = tu_strlen ( ident );
	    status = dnet_write 
		( dptr, ident, id_len > 255 ? 255 : id_len, &written );
	    break;
        }
        if ( (status&1) == 0 ) {
	    dnet_format_error ( status, buffer, bufsize > 256 ? 256 : bufsize );
	    errmsg = "500 Error writing to DECnet object";
	    dnet_disconnect ( dptr );
	    continue;
        }
	/*
	 * Read first response.
	 */
	status = dnet_read ( dptr, buffer, bufsize, &length );
	if ( (status&1) == 0 ) {
	    dnet_disconnect ( dptr );
	    errmsg = "500 Error reading command from script";
	    dnet_format_error ( status, buffer, bufsize > 256 ? 256 : bufsize );
	} else if ( (length == 10) && 
		(0 == tu_strncmp(buffer,"<DNETBUSY>",10) ) ) {
	    /*
	     * Server is refusing connection.
	     */
	    dnet_disconnect ( dptr );
	    errmsg = "500 Error connecting to script process";
	    tu_strcpy ( buffer,	"All scriptservers busy or inaccesbile" );
	    status = 20;
	}
	else break;	/* We got connection */
    }
    if ( (status&1) == 0 ) {
	/* Format last status code */
	status = http_send_error ( scb, errmsg, buffer );
	return;
    }
    /*
     * Main loop, get commands from remote task.
     */
    text_mode = 0;
    for ( ; (status&1);
	     status = dnet_read (dptr, buffer, bufsize, &length) ) {
	/*
	 * Lookup message in tag definition table.  Since all tags start
	 * with "<DNET", start compare at 6th position.
	 */
	siglen = length - 5;
	if ( 0 != tu_strncmp(buffer,"<DNET",5) ) length = sizeof(buffer)-1;
	for ( opcode = 0; tag_list[opcode].l > 0; opcode++ ) 
	    if ( (length ==tag_list[opcode].l) && (0==tu_strncmp(&buffer[5], 
			&tag_list[opcode].s[5], siglen) ) ) break;
	buffer[length] = '\0';
	if ( http_log_level > 4 ) tlog_putlog ( 4,
	    "!AZ Control message from decnet task: '!AZ'(opc: !SL) len: !SL!/", 
		    scb->log_prefix, buffer, opcode, length );
	/*
	 * Execute requested command.
	 */
	terminal = tag_list[opcode].terminal;
	opcode = tag_list[opcode].opc;
	switch ( opcode ) {
	  case DNET_HDR:	/* <DNETHDR> */
	    /*
	     * Write remaining request header lines.  Include blank one.
	     */
	    i = 3;
	    do { status = dnet_write ( dptr, scb->request[i].s, 
			scb->request[i].l > 0 ? scb->request[i].l : 0, &written );
		if ( (status &1) == 0 ) break;
	    } while ( scb->request[i++].l > 0 );

	    break;

	  case DNET_ARG:	/* <DNETARG> */
	    /*
	     * Write URL arg.
	     */
	    i = tu_strlen ( arg );
	    status = dnet_write ( dptr, arg, i, &written );
	    break;

	  case DNET_ARG2:	/* <DNETARG2> */
	    /*
	     * Write URL arg. limited to 255 chars.
	     */
	    i = tu_strlen ( arg );
	    status = dnet_write ( dptr, arg, i > 255  ? 255 : i, &written );
	    break;

	  case DNET_TEXT:	/* <DNETTEXT> */
	    /*
	     * Object will send us status line followed by simple text to be 
	     * formatted, one line per decnet message.
	     */
	    status = dnet_read(dptr, buffer, bufsize, &length);
	    if ( (status&1) == 0 ) {
		dnet_format_error ( status, buffer, 
			bufsize > 256 ? 256 : bufsize );
	        status = http_send_error ( scb,
			"500 I/O error in DECnet task", buffer );
		return status;
	    }
	    buffer[length] = '\0';
    	    status = http_add_response ( scb->rsphdr, buffer, 1 );
    	    status = http_send_response_header ( scb->cnx, scb->rsphdr );
	    /* scb->data_bytes += scb->rsphdr->l; */
	    if ( (status&1) == 0 ) break;
            status = transfer_output ( 1, dptr, scb->cnx, iobuf, 
			"</DNETTEXT>", &scb->data_bytes );
	    break;

	  case DNET_RAW:	/* <DNETRAW> */
	    /*
	     * Remote task will handle all formatting, relay raw data.
	     * Put first line in rsphdr so log file can examine it.
	     */
	    status = dnet_read ( dptr, iobuf->s,
			iobuf->l > 4096 ? 4096 : iobuf->l, &length );
    	    if ( (status&1) == 0 ) break;
	    if ( (length == 10) && 
		 ( 0 == tu_strncmp ( iobuf->s, "</DNETRAW>", 10 ) ) ) {
		scb->rsphdr->l = 0;		/* end of file */
		status = VMS_EOF;
	    } else {
		tu_strnzcpy ( scb->rsphdr->s, iobuf->s, 40 );
		scb->rsphdr->l = tu_strlen ( scb->rsphdr->s );
		/*
		 * Copy buffer to client and continue transfer till done.
		 */
		if ( length > 0 ) status = ts_tcp_write 
			( scb->cnx, iobuf->s, length );
		if ( (status&1) == 0 ) break;
		scb->data_bytes += length;

		status = transfer_output ( text_mode, dptr, scb->cnx, iobuf, 
			"</DNETRAW>", &scb->data_bytes );
	    }
	    break;

	  case DNET_RQURL:	/* <DNETRQURL> */
	    /*
	     * Write actual requested URL arg. unlimited length.
	     */
	    i = tu_strlen ( arg );
	    status = dnet_write ( dptr, scb->request[1].s, 
			scb->request[1].l, &written );
	    break;

	  case DNET_CGI:	/* <DNETCGI> */
	    /*
	     * Special 'CGI' mode, similar to RAW except redirect may take
	     * action.
	     */
	    status = http_decnet_cgi ( scb, dptr, text_mode, iobuf );
	    break;

	  case DNET_HOST:	/* <DNETHOST> */
	    /*
	     * Write HTTP_DEFAULT_HOST to server task.
	     */
	    tmp = http_default_host ? http_default_host : "???";
	    status = dnet_write ( dptr, tmp, tu_strlen(tmp), &written );
	    break;

	  case DNET_ID:		/* <DNETID> */
	  case DNET_ID2:	/* <DNETID2> */
	    /*
	     * Write software version, server name, server port, remote_port, 
	     * remote address, remote user, remote host to server task.
	     */
	    status = ts_tcp_info ( &local_port, &remote_port, 
		(unsigned int *) &remote_addr );
	    tu_strcpy ( buffer, "OSU/" ); length = tu_strlen ( buffer );
	    tu_strcpy ( &buffer[length], http_server_version );
	    length = tu_strlen ( buffer );
	    buffer[length++] = ' ';
	    tmp = http_default_host ? http_default_host : "???";
	    tu_strcpy ( &buffer[length], tmp ); 
	    length += tu_strlen ( &buffer[length] );
	    buffer[length++] = ' ';
	    tu_strint ( local_port, &buffer[length] );
	    length += tu_strlen ( &buffer[length] );
	    buffer[length++] = ' ';
	    tu_strint ( remote_port, &buffer[length] );
	    length += tu_strlen ( &buffer[length] );
	    buffer[length++] = ' ';
	    tu_strint ( remote_addr, &buffer[length] );
	    length += tu_strlen ( &buffer[length] );
	    if ( scb->acc->user[0] ) {
		buffer[length++] = ' ';
	        tu_strcpy ( &buffer[length], scb->acc->user );
	        length += tu_strlen ( &buffer[length] );
	    } else if ( opcode == DNET_ID2 ) {
		/* make placeholder for username */
		buffer[length++] = ' ';
	    }
	    if ( (opcode == DNET_ID2) && http_dns_enable ) {
		/* Append remote hostname to return string */
		buffer[length++] = ' ';
		tu_strcpy (&buffer[length], ts_tcp_remote_host() );
	        length += tu_strlen ( &buffer[length] );
	    }
	    status = dnet_write ( dptr, buffer, length, &written );
	    break;

	  case DNET_BINDIR:	/* <DNETBINDIR> */
	    /*
	     * Write HTTP_BINDIR to server task.  This is directory to
	     * search for scripts.
	     */
	    tmp = bindir ? bindir : "???";
	    i = tu_strlen ( tmp );
	    status = dnet_write ( dptr, tmp, i > 255 ? 255 : i, &written );
	    break;

	  case DNET_PATH:	/* <DNETPATH> */
	    /*
	     * Write path to server task.  This is ident prefix that
	     * caused script invocation.
	     */
	    tmp = (dir_len > 0 ) ? &ident[id_len+1] : "???";
	    i = tu_strlen ( tmp );
	    if ( i > dir_len ) i = dir_len;
	    status = dnet_write ( dptr, tmp, i > 255 ? 255 : i, &written );
	    break;

	  case DNET_INPUT:
	    /*
	     * Read characters from client and relay to server task.
	     */
	    status = tu_read_raw ( scb->inbound, buffer, 254, &length );
	    if ( (status&1) == 1 ) {
	        if ( http_log_level > 9 ) tlog_putlog ( 9,
		   "!AZ Data bytes read from client: !SL!/", scb->log_prefix,
		    length );
	        status = dnet_write ( dptr, buffer, length,  &written );
	    }
	    break;

	  case DNET_XLATE:
	    /*
	     * Read test URL from task and translate.
	     */
	    status = dnet_read (dptr, buffer, bufsize, &length);
	    if ( (status&1) == 0 ) break;
	    if ( length > 256 ) length = 256;
	    buffer[length] = '\0';
	    tmp = &buffer[length+1];		/* Allocate temp from buffer*/
	    status = http_translate_ident(buffer, tmp, bufsize - length, &acc);
	    if ( http_log_level > 4 )
		tlog_putlog ( 4, 
			"!AZ URL translate status: !SL maxlen: !SL!/", 
			scb->log_prefix, status, bufsize - length );
	    if ( status == 0 ) {
		/* Translation failed, return null string */
		buffer[length+1] = '\0';
	    }
	    length = tu_strlen ( tmp );
	    status = dnet_write ( dptr, tmp, length, &written );
	    break;

	  case DNET_INVCACHE:
	    /* mark invalid */
	    http_invalidate_doc_cache();	/* clear cache */
	    status = 1;
	    break;

	  case DNET_RECMODE:
	    /* Force text mode */
	    text_mode = 1;
	    status = 1;
	    break;

	  default:
	    /*
	     * Protocol error.
	     */
	    buffer[length] = '\0';
	    status = http_send_error ( scb,
		"500 Protocol error in DECnet task", buffer );
	    scb->data_bytes += scb->rsphdr->l;
	    break;
	}
	if ( (status&1) == 0 ) break;	/* abort */
	if ( terminal ) break;
    }
    /*
     * Send last-chance error message if nothing sent so far.
     */
    if ( scb->rsphdr->l <= 0 ) {
	int fallback_status;
	if ( (status&1) == 0 ) dnet_format_error ( status, buffer, 256 );
	else tu_strcpy ( buffer, 
		"Script produced no output, check NETSERVER.LOG" );
	fallback_status = http_send_error ( scb,
		"500 protocol error in DECnet object", buffer );
    }
    dnet_disconnect ( dptr );
    return status;
}
/**************************************************************************/
/* Special read routines for reading CGI response header lines.  If the
 * end-of-response tag (</DNETCGI>) is read, replace status with EOF
 * so that tu_readline will abort read (otherwise readline would try
 * to read more data since tag does not include a linefeed).
 *
 * In record mode (text mode) read_cgi_recmode_header is used to read the
 * header lines, it appends a newline to every record.
 */
static int read_cgi_header 
	( void *dptr, char *buffer, int bufsize, int *read )
{
    int status;
    status = dnet_read ( dptr, buffer, bufsize, read );
    if ( (status&1) == 0 ) return status;
    if ( (*read == 10) && (*buffer == '<') ) {
	if ( 0 == tu_strncmp ( buffer, "</DNETCGI>", 10 ) ) status = VMS_EOF;
    }
    return status;
}
static int read_cgi_recmode_header 
	( void *dptr, char *buffer, int bufsize, int *read )
{
    int status;
    if ( bufsize <= 2 ) return read_cgi_header ( dptr, buffer, bufsize, read );
    status = dnet_read ( dptr, buffer, bufsize-2, read );
    if ( (status&1) == 0 ) return status;
    if ( (*read == 10) && (*buffer == '<') ) {
	if ( 0 == tu_strncmp ( buffer, "</DNETCGI>", 10 ) ) status = VMS_EOF;
    }
    buffer[*read] = '\r'; 
    *read += 1;
    buffer[*read] = '\n';	/* Add implied CR/LF */
    *read += 1;
    return status;
}
/**************************************************************************/
/*
 * Handle processing of CGI mode script execution.  Read header lines from
 * from script and take action.  Note that CGI input is a stream, records
 * are delimited by <CR><LF> or <LF>.
 */
static int http_decnet_cgi 
	( session_ctx scb,		/* session control block */
	  void *dptr, 			/* Decnet connection to script task */
	  int text_mode,		/* true if implied CRLF */
	  string *iobuf )		/* Scratch data buffer for I/O */
{
    int status, length, i, lines, mode, tcnt, bufsize, http_parse_elements();
    int http_send_document(), http_translate_ident(), tlog_putlog();
    struct acc_info_struct acc, *orig_acc;
    struct tu_streambuf input, *orig_input;
    char *buffer, label[80], location[1024], content[256], stsline[256];
    /*
     * Use utitlity routine to parse stream.
     */
    bufsize = iobuf->l; buffer = iobuf->s;
    tu_init_stream ( dptr, 
	text_mode ? read_cgi_recmode_header : read_cgi_header, &input );
    stsline[0] = '\0';

    for ( mode=0; ; ) {
	/*
	 * Read next header line, end loop until null line read.
	 */
        status = tu_read_line ( &input, buffer, bufsize-1, &length );
	if ( (status&1) == 0 ) tlog_putlog ( 0,
	    "!AZ CGI readline error: !SL!/", scb->log_prefix, status );
	if ( (status&1) == 0 ) return status;
	buffer[length] = '\0';		/* Ensure we have terminated string */
	if ( http_log_level > 4 )
	    tlog_putlog ( 4, "!AZ CGI header: '!AZ' (!SL)!/",
		scb->log_prefix, buffer, length );
	if ( length == 0 ) break;	/* header terminator */
	/*
	 * parse first element and make sure there are at least 2.
	 */
	for ( i = 0; (i < length) && (i < sizeof(label)); i++ ) {
	    label[i] = buffer[i];
	    if ( label[i] == ':' ) break;
        }
	if ( (i >= length) || (i >= sizeof(label)) ) return 20;
	label[i+1] = '\0';
	for ( i++; (buffer[i] == ' ') || (buffer[i] == '\t'); i++ );
	if ( buffer[i] == '\0' ) return 20;	/* missing argument */
	/*
	 * Interpret label to see if it is CGI directive.
	 */
	tu_strupcase ( label, label );
	if ( tu_strncmp ( label, "LOCATION:", 9 ) == 0 ) {
	    /*
	     * Save next element in location array.
	     */
	    tu_strnzcpy ( location, &buffer[i], sizeof(location)-1 );
	    mode = 1;
	} else if ( tu_strncmp ( label, "CONTENT-TYPE:", 13) == 0 ) {
	    /*
	     * Save next element in content header line.
	     */
	    tu_strcpy ( content, "Content-type: " );
	    tu_strnzcpy ( &content[14], &buffer[i], sizeof(content)-14 );
	    mode = 2;
	} else if ( tu_strncmp ( label, "STATUS:", 13) == 0 ) {
	    /*
	     * Override status line to return.
	     */
	    tu_strnzcpy ( stsline, &buffer[i], sizeof(stsline)-1 );
	} else if ( mode >= 2 ) {
	    /*
	     * Additional headers after a content-type header are saved.
	     */
	    if ( mode == 2 ) {
		/* Make first line of response the current status line */
		if ( !stsline[0] ) tu_strcpy ( stsline, 
			"200 OK, CGI script output, extended" );
    		status = http_add_response ( scb->rsphdr, stsline, 0 );
	    }
	    mode = 3;
	    status = http_add_response ( scb->rsphdr, buffer, 0 );
	    
	} else {
	    /*
	     * Illegal value, first line must be directive.
	     */
	    return 20;
	}
    }
    /*
     * Getting to this point means we have a valid header.  Continue
     * processing based upon mode (which type of header).
     */
    if ( http_log_level > 4 ) tlog_putlog ( 4, 
	"!AZ CGI mode: !SL,  stm buf: !SL/!SL!/", scb->log_prefix,
		 mode, input.used, input.filled );
    if ( mode == 0 ) {
	tu_strcpy ( stsline, "500 Script protocol error, no content-type" );
	tu_strcpy ( content,"content-type: text/plain" );
	mode = 2;
    }
    if ( mode >= 2 ) {
	/*
	 * Script will return data of indicated contents type.  Build
	 * header with indicated content type and send to client.
	 */
	if ( mode == 2 ) {
	    if ( !stsline[0] ) 
		tu_strcpy ( stsline, "200 OK, CGI script output" );
    	    status = http_add_response ( scb->rsphdr, stsline, 0 );
	}
	status = http_add_response ( scb->rsphdr, "MIME-version: 1.0", 0 );
	tu_strcpy ( stsline, "Server: OSU/" );
	tu_strcpy ( &stsline[12], http_server_version );
    	status = http_add_response ( scb->rsphdr, stsline, 0 );
	status = http_add_response ( scb->rsphdr, content, 0 );
	status = http_send_response_header ( scb->cnx, scb->rsphdr );
	if ( (status&1) == 0 ) 
	    tlog_putlog ( 0, "!AZ Error sending CGI header to client: !SL!/",
			scb->log_prefix, status);
	if ( (status&1) == 0 ) return status;
	/*
	 * Dump anything left in stream buffer to client.
	 */
	if ( input.used && (input.used < input.filled) ) {
	    status = tu_read_raw ( &input, buffer, bufsize, &length );
	    if ( (status&1) == 1 ) status = ts_tcp_write 
			( scb->cnx, buffer, length );
	    else if ( status == VMS_EOF ) return 1;

	    if ( (status&1) == 0 ) {
	        tlog_putlog ( 0, "!AZ Error flushing readline buffer: !SL !SL!/",
			scb->log_prefix, status, bufsize);
	        input.used = input.filled; 
	        length = 0; 
	    }
	    scb->data_bytes += length;
	}
        /*
	 * Transfer rest of task's output in raw mode.
	 */
        status = transfer_output ( text_mode, dptr, 
		scb->cnx, iobuf, "</DNETCGI>", &scb->data_bytes );
	return status;
    }
    if ( mode == 1 ) {
	/*
	 * Script is supplying a relocation.  Determine if it is relative
	 * or absolute.  Location is absolute if it contains a scheme (colon),
	 * a node (//), or a tag (#).
	 */
	int i, absolute, http_parse_url();
	char *scheme, *node, *ident, *arg, url_store[1024];

	status = http_parse_url ( location, url_store, &scheme, &node,
		&ident, &arg );
	/*printf("url parse: '%s' '%s' '%s' '%s'\n", scheme, node,ident,arg);*/
	absolute = 0;
	if ( *scheme || *node || *arg ) {
	    /*
	     * Send redirection to be handled by the client.
	     */
	    if ( !stsline[0] ) tu_strcpy ( stsline, "302 OK, CGI redirect" );
    	    status = http_add_response ( scb->rsphdr, stsline, 0 );
	    tu_strcpy ( buffer, "location: " );
	    tu_strcpy ( &buffer[10], location );
	    status = http_add_response ( scb->rsphdr, buffer, 1 );
	    status = http_send_response_header ( scb->cnx, scb->rsphdr );
	    if ( (status&1) == 0 ) return status;
	    scb->data_bytes += scb->rsphdr->l;
	    return status;
	} else {
	    /*
	     * Relative file, recursively call send-document.
	     */
	    string orig_ident;
	    char munged_ident[512];
	    /*
	     * Translate ident by rules file, mainly to do access check.
	     */
	    if ( http_log_level > 4 ) tlog_putlog ( 
		"!AZ CGI Recursively fetching relative ident: !AZ!/", 
		scb->log_prefix, ident );

            status = http_translate_ident ( ident, munged_ident, 
			sizeof(munged_ident), &acc );
	    if ( !status ) {
	        /*
	         * Error status means ident matched a 'fail' record in the
	         * the rule database.
	         */
	        status = http_send_error ( scb, "403 Forbidden",
		    "Access to object is _ruled_ out." );
	        return status;
	    }
	    /*
	     * Save original request ident and re-call send_document
	     * with the re-directed one.
	     */
	    orig_ident = scb->request[1];
	    orig_input = scb->inbound;
	    orig_acc = scb->acc;
	    scb->inbound = &input;
	    scb->request[1].l = tu_strlen(ident); scb->request[1].s = ident;
	    scb->acc = &acc;
	    status = http_send_document 
			( scb, ident, munged_ident, arg, iobuf );
	    scb->request[1] = orig_ident;
	    scb->inbound = orig_input;
	    scb->acc = orig_acc;
	    return status;
	}
    }
    return 1;
}
