/*
 * This module is the main code for processing a request received by
 * the http server.
 *
 * The request is specifed by an array of structures that point to strings.
 * The first 3 elements point to the 3 parts of the initial request line
 * (method, URL, protocol-version).  The remaining elements point to
 * header request lines.  This list is always terminated by a zero-length
 * line.  If the caller detected an error while generating the request,
 * the final line will have a negative length.
 *
 * Author: 	David Jones
 * Revised: 	24-JUN-1994	! introduced session control block.
 * Revised:	17-SEP-1994	! remove support of INVCACHE method (plenty
 *				  of other means available).
 * Revised:	25-sep-1994	Added tserver_tcp.h header file.
 * Revised:	11-OCT-1994	Add content-transfer-encoding to standard rsp.
 * Revised:	15-FEB-1995	Change name of decnet_execute to script_execute
 */
#include <stdio.h>
#include "session.h"		/* Session control block definition */
#include "tserver_tcp.h"	/* TCP/IP I/O routines */

int http_log_level;		/* Global variable, logging control */
int tlog_putlog();			/* Logging output routine */
int http_dns_enable;		/* GLobal variable, name lookup enable */
extern char http_server_version[];	/* Current server version */
int http_add_response(), http_send_response_header();

int http_execute ( void *ctx,		/* opaque, context for tcp connection */
	char *log_prefix,		/* Tag for log file output */
	string *request,		/* Request to process. */
	tu_stream inbound )		/* Input character stream */
{
    int status, http_send_error(), http_send_document(), log_access();
    int http_parse_url(), http_translate_ident(), http_script_execute();
    char *service, *node, *ident, *suffix, *arg;
    struct session_control_block scb;
    struct acc_info_struct acc;
    char url_buf[4096], rsphdrtxt[512], munged_ident[300], iobuffer[4096];
    string iobuf;
    struct tu_textbuf rsphdr;
    /*
     * Initialize structure for holding response header lines.
     */
    rsphdr.l = 0;			/* current length */
    rsphdr.s = rsphdrtxt;		/* data storage */
    rsphdr.size = sizeof(rsphdrtxt);	/* Storage capacity */
    iobuf.l = sizeof(iobuffer);		/* Work area */
    iobuf.s = iobuffer;			/* Work area */
    if ( request[2].l > 0 ) tu_add_text ( &rsphdr, "HTTP/1.0 ", 9 );
    /*
     * Build session control block, init access control structure.
     */
    scb.cnx = ctx;
    scb.request = request;
    scb.inbound = inbound;
    scb.rsphdr = &rsphdr;
    scb.data_bytes = 0;
    scb.acc = &acc;
    scb.log_prefix = log_prefix;
    scb.completed = 0;
    acc.uic = 0;
    acc.cache_allowed = 1;
    acc.prot_file = "";
    acc.rem_user[0] = acc.user[0] = '\0';
    /*
     * Handle error case.
     */
    if ( request[3].l < 0 ) {
	if ( http_log_level > 1 ) tlog_putlog 
		( 1, "!AZ generating error response!/", log_prefix );
	if ( request[3].l == -2 ) {
	    status = http_send_error ( &scb, "580 Server overload",
		"Request rejected due to server congestion" );
	} else {
	    status = http_send_error ( &scb, "400 Bad request",
		"Syntax error or malformed request" );
	}
	return status;
    }
#ifdef SHOW_HEADERS
    if ( http_log_level > 2 ) {
	int i;
	tlog_putlog ( 2, "!AZ Headers: !/", log_prefix );
	for ( i = 3; request[i].l > 0; i++ ) tlog_putlog ( 2,
		"!AZ    !AF!/", log_prefix, request[i].l, request[i].s );
	tlog_putlog( 2, "!/" );
    }
#endif
    /*
     * First attempt to translate URL.
     */
    status = http_parse_url ( request[1].s, url_buf, &service, &node, &ident, 
	&arg );
    if ( status ) {
	/*
	 * Check for gateway request (node field is non-blank).
	 */
	if ( *service || *node ) {
	    status = http_send_error ( &scb, "501 Not implemented",
		"Gateway function not available." );
	    return status;
	}
	/*
	 * Local ident, attempt to re-map according to local rules.
	 */
        status = http_translate_ident ( ident, munged_ident, 
			sizeof(munged_ident), scb.acc );
	if ( !status ) {
	    /*
	     * Error status means ident matched a 'fail' record in the
	     * the rule database.
	     */
	    status = http_send_error ( &scb, "403 Forbidden",
		"Access to object is _ruled_ out." );
	    return status;
	}
	
    } else {
	/*
	 * Parse failure on URL.
	 */
	status = http_send_error 
		( &scb, "400 Bad URL","invalid URL specified" );
	return status;
    }
    /*
     * Take action depending upon method specified in request or punt
     * to HTBIN escape mechanism.
     */
    if ( status == 2 ) {
	/* Rule file redirect */
	tu_strcpy ( iobuffer, "location: " );
	tu_strcpy ( &iobuffer[10], munged_ident );
	status = http_add_response ( &rsphdr, "302, rule file redirect", 0 );
	status = http_add_response ( &rsphdr, iobuffer, 1 );
	status = http_send_response_header ( ctx, &rsphdr );

    } else if ( status == 3 ) {
	/* Exec command. */
	status = http_script_execute 
		( &scb, "HTBIN", munged_ident, arg, &iobuf );
    } else if ( 0 == tu_strncmp ( request[0].s, "GET", 4 ) ) {
        status = http_send_document (&scb, ident, munged_ident, arg, &iobuf);

    } else if ( 0 == tu_strncmp ( request[0].s, "HEAD", 5 )  ) {
        status = http_send_document (&scb, ident, munged_ident, arg, &iobuf);

    } else if ( 0 == tu_strncmp ( request[0].s, "POST", 5 )  ) {
        status = http_script_execute 
		( &scb, "POST", munged_ident, arg, &rsphdr, &iobuf );

    } else {
	status = http_send_error ( &scb, "501 Not implemented",
		"Method not supported by this server" );
    }
    /*
     * Log data transfer statistics.
     */
    if ( http_log_level > 2 )
    tlog_putlog ( 2, "!AZ      response stats: hdr = !SL, data = !SL bytes!/",
	log_prefix, rsphdr.l, scb.data_bytes );
    log_access ( &scb );
    return status;
}
#ifndef VMS
/**************************************************************************/
/* Temporary stub routine */
int http_script_execute (
	session_ctx scb,		/* Session control block */
	char *subfunc, 			/* Subfunction to execute */
	char *ident,			/* Ident parsed from request. */
	char *arg,			/* Search argument portion of URL */
	string *iobuf )			/* I/O buffer */
{
    return 36;
}
#define SYS$NUMTIM SYS_NUMTIM
#endif
/**************************************************************************/
/*
 * Generate summary log line in common logfile format.
 * Format:
 * 	remotehost rfc931 authuser [date] "request" status bytes 
 */
int log_access ( session_ctx scb )
{
    char *stat_code, *hostname;
    unsigned char host[4];
    int local_port, remote_port, SYS$NUMTIM(), stspos;
    string *request;
    short int time[8];
    static char *month_names[12] = { "Jan", "Feb", "Mar", "Apr", "May",
		"Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

    if ( scb->completed ) return 0;	/* Already logged */
    scb->completed = 1;			/* Mark as logged */
    /*
     * Locate HTTP response within the header (skip version field of
     * response line).  If no status, use 000.
     */
    request = scb->request;
    stspos = request[2].l > 0 ? 9 : 0;
    if ( stspos+3 >= scb->rsphdr->l ) {
	stspos = scb->rsphdr->l+1;
	tu_strcpy ( &scb->rsphdr->s[stspos], "000" );
    }
    /*
     * Format time request completed.
     */
    SYS$NUMTIM ( time, 0 );		/* Time request is logged */
    ts_tcp_info ( &local_port, &remote_port, (unsigned int *) host );
			/* 4*4 + 40 + 30 + 4 + 3 + 10 */

    if ( http_dns_enable ) {
	if ( http_dns_enable == 2 ) {
	} else {
	    hostname = ts_tcp_remote_host();
	}
	tlog_putlog ( -1,
        "!AZ - !AZ [!2ZW/!AD/!4UW:!2ZW:!2ZW:!2ZW +0000] \"!AF !AF !AF\" !AF !SL!AZ",
	hostname,
	scb->acc->user[0] ? scb->acc->user : "-",  /* local user */
	time[2], 3, month_names[time[1]-1], time[0], time[3], time[4], time[5],
	request[0].l > 30 ? 30 : request[0].l, request[0].s,
	request[1].l > 300 ? 300 : request[1].l, request[1].s,
	request[2].l > 30 ? 30 : request[2].l, request[2].s, 
	3, &scb->rsphdr->s[stspos], scb->data_bytes, "\n" );
    } else {
        tlog_putlog ( -1,
        "!UB.!UB.!UB.!UB - !AZ [!2ZW/!AD/!4UW:!2ZW:!2ZW:!2ZW +0000] \"!AF !AF !AF\" !AF !SL!AZ",
	host[0], host[1], host[2], host[3],            /* Remote host */
	scb->acc->user[0] ? scb->acc->user : "-",  /* local user */
	time[2], 3, month_names[time[1]-1], time[0], time[3], time[4], time[5],
	request[0].l > 30 ? 30 : request[0].l, request[0].s,
	request[1].l > 300 ? 300 : request[1].l, request[1].s,
	request[2].l > 30 ? 30 : request[2].l, request[2].s, 
	3, &scb->rsphdr->s[stspos], scb->data_bytes, "\n" );
    }
    return 1;
}
/**************************************************************************/
/* Append text followed by CRLF to response buffer.   If standard flag is true,
 *  append default headers to buffer. If standard flag is 2, use html content.
 */
int http_add_response ( tu_text rsp, char *text, int standard )
{
    int status;
    status = tu_add_text ( rsp, text, 512 );
    status = tu_add_text ( rsp, "\r\n", 3 );
    if ( standard ) {
	status = tu_add_text ( rsp, "MIME-version: 1.0\r\n", 80 );
	status = tu_add_text ( rsp, "Server: OSU/", 80 );
	status = tu_add_text ( rsp, http_server_version, 80 );
	status = tu_add_text ( rsp, (standard == 2) ? 
		"\r\nContent-type: text/html\r\n" : 
		"\r\nContent-type: text/plain\r\n", 80 );
	status = tu_add_text (rsp, "Content-transfer-encoding: 8bit\r\n", 80);
    }
    return status;
}
/*****************************************************************************/
/*  Generate plain/text document to report error and send to client.
 */
int http_send_error ( session_ctx scb, char *status_msg, char *err_text )
{
    int i, status;
    void *cnx;
    string *request;
    /*
     * Build and send standard response header.
     */
    cnx = scb->cnx;
    request = scb->request;
    status = http_add_response ( scb->rsphdr, status_msg, 1 );
    status = http_send_response_header ( cnx, scb->rsphdr );
    if ( (status&1) == 1 ) {
	/*
	 * Send additional error text as body of message.
	 */
	status = ts_tcp_write ( cnx,"-ERROR-(", 8 );
	status = ts_tcp_write ( cnx, status_msg, 3 );
	status = ts_tcp_write ( cnx, "):  ", 4 );
	status = ts_tcp_write ( cnx, err_text, tu_strlen(err_text) );
	if ( (status&1) == 0 ) return status;
	/*
	 * Send initial request parse result.
	 */
	if ( request[0].l > 0 ) {
	    status = ts_tcp_write ( cnx, "\r\nRequested method: ", 20 );
	    if ( (status&1) == 0 ) return status;
	    status = ts_tcp_write ( cnx, request[0].s, request[0].l );
	}
	if ( request[1].l > 0 ) {
	    status = ts_tcp_write ( cnx, "\r\nRequested URL:    ", 20 );
	    if ( (status&1) == 0 ) return status;
	    status = ts_tcp_write ( cnx, request[1].s, request[1].l );
	}
	if ( request[2].l > 0 ) {
	    status = ts_tcp_write ( cnx, "\r\nHTTP protocol:    ", 20 );
	    if ( (status&1) == 0 ) return status;
	    status = ts_tcp_write ( cnx, request[2].s, request[2].l );
	}
	/*
	 * Send any remaining request lines.
	 */
	status = ts_tcp_write ( cnx, 
	/*               5678901234567890123456789012345678901234567890 */
		"\r\n\r\n-------- additional request headers --------\r\n",50);
	for ( i = 3; (request[i].l > 0) && (status&1); i++ ) {
	    status = ts_tcp_write ( cnx, request[i].s, request[i].l );
	    if ( (status&1) == 0 ) return status;
	    status = ts_tcp_write ( cnx, "\r\n", 2 );
	    if ( (status&1) == 0 ) return status;
	}
    }
    log_access ( scb );			/* enter into log file */
    return status;
}
/***************************************************************************/
/* Send contents of response header buffer to client.
 */
int http_send_response_header ( void *ctx, tu_text rsp)
{
    int status, length;
    char *buffer;

    buffer = rsp->s;
    if ( (rsp->l > 9) && (0 == tu_strncmp(rsp->s,"HTTP/1.0 ", 9)) ) {
	tu_add_text ( rsp, "\r\n", 3 );		/* add null line */
	length = rsp->l;
    } else if ( *buffer == '3' ) {  /* force full re-direct headers */
	/*
	 * Response buffer is old protocol, skip the version field and
	 * truncate after first line.
	 */
	/* buffer = &buffer[9]; */
	for ( length = 0; 
		buffer[length] && (buffer[length] != '\n'); length++);
	if ( buffer[length] == '\n' ) length++;
	buffer = &buffer[length];
	for ( ; buffer[length] && (buffer[length] != '\n'); length++);
	if ( buffer[length] == '\n' ) length++;
    } else {
	/*
	 * Don't send anything.
	 */
	length = 0;
    }
    if ( length > 0 ) status = ts_tcp_write ( ctx, buffer, length );
    else status = 1;
    return status;
}
/***************************************************************************/
int http_parse_url 
	( char *url, 			/* locator to parse */
	char *info,			/* Scratch area for result pts*/
	char **service,			/* Protocol (e.g. http) indicator */
	char **node,			/* Node name. */
	char **ident,			/* File specification. */
	char **arg )			/* Search argument */
	
{
    int i, state;
    char *last_slash, *p, c, arg_c;
    /*
     * Copy contents of url into info area.
     */
    *service = *node = *ident = *arg = last_slash = "";

    for ( state = i = 0; (info[i] = url[i]) != 0; ) {
	c = info[i];
	switch ( state ) {
	    case 0:
		if ( c == ':' ) {
		    info[i] = '\0';	/* terminate string */
		    *service = info;
		    state = 1;
		}
	    case 1:
		if ( c == '/' ) {
		    *ident = last_slash = &info[i];
		    state = 2;
		}
		break;
	    case 2:
		state = 4;
		if ( c == '/' ) {	/* 2 slashes in a row */
		    *node = *ident;
		    state = 3;
		}
		else if ( (c == '#') || (c == '?') ) {
		    arg_c = c;
		    info[i] = '\0';
		    *arg = &info[i+1];
		    state = 5;
		}
		break;
	    case 3:			/* find end of host spec */
		if ( c == '/' ) {
		    state = 4;
		    *ident = last_slash = &info[i];
		    for ( p = *node; p < *ident; p++ ) p[0] = p[1];
		    info[i-1] = '\0';	/* Terminate host string */
		}
		break;
	    case 4:			/* Find end of filename */
		if ( c == '/' ) last_slash = &info[i];
		else if ( (c == '#') || (c == '?') ) {
		    arg_c = c;
		    info[i] = '\0';
		    *arg = &info[i+1];
		    state = 5;
		}
	    case 5:
		break;
        }
	i++;
    }
    /*
     * Insert arg delimiter back into string.
     */
    if ( **arg ) {
	char tmp;
	for ( p = *arg; arg_c; p++ ) { tmp = *p; *p = arg_c; arg_c = tmp; }
	*p = '\0';
    }
    return 1;
}
/*
 * Scan ident and return pointer to suffix porition of filename, including
 * the period.
 * Return "/" if no filename is present and "" if no extension.
 */
char *http_url_suffix ( char *ident )
{
    char *p, *suffix;
    suffix = "";
    for ( p = ident; *p; p++ )
	if ( *p == '.' ) suffix = p;
	else if ( *p == '/' ) suffix = "";
    /*
     * Check if last thing on line is "/".
     */
    if ( (*suffix == '\0') && (p > ident) )
	if ( *--p == '/' ) suffix = p;

    return suffix;
}
/***************************************************************************/ 
