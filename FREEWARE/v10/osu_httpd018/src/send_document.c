/*
 * Main code for formating and transmiting a file to a WWW client.
 *
 * Author:	David Jones
 * Revised:	26-MAY-1994		Updated HTTP_GET_DOC_CACHE call.
 * Revised:	24-JUN-1994		Introduce session control block.
 * Revised:	22-JUL-1994		Explicitly close file on HEAD command.
 * Revised:	14-AUG-1994		validate file ownership.
 * Revised:	24-AUG-1994		Re-work presentation action.
 * Revised:	29-AUG-1994		Include file count in dir_to_html.
 * Revised:	12-OCT-1994		Include Transfer encoding.
 * Revised:	 4-NOV-1994		Replace check for index.html with
 *					search list.
 * Revised:     16-NOV-1994		Support DirAccess directive.
 * Revised:	22-NOV-1994		Support fallback check for directories.
 * Revised:	17-DEC-1994		Add support for if-modified-since.
 * Revised:     26-DEC-1994		Include last-modified header on 304
 *					response for Netscape's sake.
 * Revised:	14-JAN-1995		Add hack for content-encoding.
 * Revised:	15-FEB-1995		Change name to script_execute
 */
#include <stdio.h>
#include <ctype.h>
#include "session.h"
#include "file_access.h"
#include "tserver_tcp.h"

extern char http_server_version[];
int http_log_level;
char *http_search_script;
char *http_default_host;		/* Host name for re-directs */
char **http_index_filename;		/* List of filenames */
int http_dir_access;			/* 0-all, 1-restricted, 2-none */
char *http_dir_access_file;		/* Directory access filename */
int http_send_error(), http_add_response(), http_send_response_header();
int http_check_protection(), http_script_execute();
char *http_url_suffix();
static int extract_header_field ( char *field, string *request,
	char *buffer, int *result_length );	/* forward reference */
static int dir_to_html ( void *ctx, void *of, char *buffer, int bufsize,
	int *data_bytes );

/****************************************************************************/
int http_send_document (
	session_ctx scb,			/* Session control block */
	char *req_ident,			/* Filename in request */
	char *ident, 				/* Translated File spec. */
	char *arg,				/*  search arguments on URL */
	string *iobuf )				/* I/O buffer */
{
    int text_mode, header_only, i, status, pl, length, bufsize, enc_len;
    int http_get_doc_cache(), http_put_doc_cache(), http_match_suffix();
    int http_get_presentation(), http_check_cache_access();
    unsigned int since_time;
    void *of, *o2f;				/* tf_* file pointer */
    char *presentation;
    char *rep, *encoding, *suffix, *stsmsg, hdrline[64], *buffer, fixup[340];
    /*
     * Check access protection.
     */
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
    } else http_check_cache_access ( scb->acc );
    /*
     * Negotiate representation and encoding values based on client's
     * Capabilities.  All clients handle text/plain and text/html.
     */
    bufsize = iobuf->l; buffer = iobuf->s;
    if ( scb->request[2].l == 0 ) {
	/* 
	 * No protocol, assume HTTP/0.9 with and implicit Accept: * / *
	 */
	tu_strcpy ( buffer, "*,*/*" ); length = 5;
    } else {
	/*
	 * Default to text/html and text/plain and append any additional
	 * accepted representations specified in request header.
	 */
	tu_strcpy ( buffer, "text/html,text/plain" );
	length = 20;
	extract_header_field ( "accept", scb->request, buffer, &length );
        buffer[length] = '\0';
    }
    suffix = http_url_suffix ( ident );
    encoding = "8bit"; enc_len = 4;
    if ( *suffix == '/' ) {
	/*
	 * Requesting directory.  Use special representation so that
	 * people can write their own scripts to handle it.
	 */
	rep = "text/file-directory";
     } else {
	/*
	 * Find representation of suffix that client supports.
	 */
	rep = "text/plain";
        status = http_match_suffix ( suffix, buffer, &rep, &encoding );
	if ( (status == 0) && (*suffix == '.') ) {
	    status = http_match_suffix ( "*.*", buffer, &rep, &encoding );
	    if ( (status == 1) && (0 == tu_strncmp(encoding,"*",2)) ) {
		status = http_match_suffix ( suffix, "*/*", &rep, &encoding );
	    }
	}
	if ( status == 0 ) {
	    status = http_match_suffix ( "*", buffer, &rep, &encoding );
	    if ( (status == 1) && (0 == tu_strncmp(encoding,"*",2)) ) {
		status = http_match_suffix ( suffix, "*/*", &rep, &encoding );
	    }
	}
	if ( status == 0 ) if ( tu_strncmp("HEAD",scb->request[0].s,5) == 0 ) {
	    /*
	     * For HEAD requests, accept it anyway.
	     */
	    status= http_match_suffix ( suffix, "*/*", &rep, &encoding );
	}
        if ( status == 0 ) {
	    status = http_send_error ( scb,"406 Unsupported format",
		"Client does not accept data type" );
	    return status;
	}
    }
    /*
     * Get length of encoding string, breaking on slash (part after slash
     * is content-encoding).
     */
    for ( enc_len = 0; 
	encoding[enc_len] && (encoding[enc_len]!='/'); enc_len++ );
    /*
     * Check for defined presentation and searches.
     */
    if ( http_get_presentation ( rep, &presentation ) ) {
	/*
	 * Dynamic presentation defined for this content-type, construct
	 * exec-like ident for script_execute: url*rep*script
	 */
	tu_strnzcpy(fixup,ident,256);
	pl = tu_strlen(fixup);
	fixup[pl++] = '*';
	tu_strnzcpy(&fixup[pl],rep,sizeof(fixup)-pl-1);
	pl = tu_strlen(fixup);
	fixup[pl++] = '*';
	tu_strnzcpy(&fixup[pl],presentation,sizeof(fixup)-pl-1);
	status = http_script_execute ( scb, "CONVERT", fixup, arg, iobuf );
	return status;
    }
    if ( *arg ) {	/* Search argument present */
	if ( !http_search_script ) {
	    status = http_send_error ( scb, "500 disabled",
		"Search capability not enabled" );
	} else {
	    /* Construct exec-like argument */
	    tu_strnzcpy(fixup,ident,256);
	    pl = tu_strlen(fixup);
	    fixup[pl++] = '*'; fixup[pl++] = '*';
	    tu_strnzcpy(&fixup[pl],http_search_script,sizeof(fixup)-pl-1);
	    status = http_script_execute ( scb, "SEARCH", fixup, arg, iobuf );
	}
	return status;
    }
    /*
     * Scan for if-modified-since.
     */
    length = 0;
    since_time = 0;
    header_only = (0 == tu_strncmp("HEAD",scb->request[0].s,5));
    extract_header_field ( "if-modified-since", scb->request, buffer, &length );
    if ( length > 0 ) {
	/*
	 * Convert time to ctime for comparison purposes.
	 */
	int tlog_putlog();
	buffer[length] = '\0';
	if ( http_log_level > 1 ) 
	   tlog_putlog ( 2, "Detected if-modified since: '!AZ' (!UL)!/", buffer,
		since_time );
	since_time = tf_decode_time ( buffer );
    }
    /*
     * See if document is in cache.  If-modified-since if present will force 
     * bypass of cache.
     */
    else if ( scb->acc->cache_allowed && http_get_doc_cache ( ident, 
		rep, encoding, buffer, bufsize, &scb->data_bytes ) ) {
	/*
	 * Found cached copy.  Build header.
	 */
	http_add_response ( scb->rsphdr, 
		"200 Sending Document (from cache)", 0 );
        http_add_response (  scb->rsphdr, "MIME-version: 1.0", 0 );
	tu_strcpy ( hdrline, "Server: OSU/" );
	tu_strcpy ( &hdrline[12], http_server_version );
	http_add_response ( scb->rsphdr, hdrline, 0 );
	if ( *suffix == '/' ) rep = "text/html";
	tu_strcpy ( hdrline, "Content-type: " );
	tu_strcpy ( &hdrline[14], rep );
	http_add_response ( scb->rsphdr, hdrline, 0 );
        for (enc_len = 0; encoding[enc_len] && (encoding[enc_len]!='/'); 
		enc_len++);
	tu_strcpy ( hdrline, "Content-transfer-encoding: " );
	tu_strnzcpy ( &hdrline[27], encoding, enc_len );
	http_add_response ( scb->rsphdr, hdrline, 0 );
	if ( encoding[enc_len] == '/' ) {
	    tu_strcpy ( hdrline, "Content-encoding: " );
	    tu_strcpy ( &hdrline[18], &encoding[enc_len+1] );
	    http_add_response ( scb->rsphdr, hdrline, 0 );
	}
	/*
	 * send header and document to client.  Only send header if method is
	 * HEAD.
	 */
	status = http_send_response_header ( scb->cnx, scb->rsphdr );
	if ( ((status&1) == 1) && (scb->data_bytes > 0) ) {
	    if ( header_only ) { scb->data_bytes = 0; return status; }
	    status = ts_tcp_write ( scb->cnx, buffer, scb->data_bytes );
	}
	return status;
    }
    /*
     * Try to open file.
     */
    text_mode = tu_strncmp ( encoding, "binary", 6 );  /* non-zero if not equal*/
    if ( text_mode == 0 ) {
	/* Open file for binary access */
	stsmsg = "200 Sending data";
	of = tf_open ( ident, "rb", buffer );
    } else if ( *suffix == '/' ) {
	/*
	 * Open file for special directory listing.
	 */
	rep = "text/html";
	stsmsg = "200 Sending directory listing";
	of = tf_open ( ident, "d", buffer );
	if ( of ) {
	    char index_fname[256];
	    int length, mfd;
	    /*
	     * See if index file from list exists in same directory.
	     */
	    tu_strnzcpy ( index_fname, ident, 255 );
	    length = tu_strlen ( index_fname );
	    for ( o2f = (void *) 0, i = 0; http_index_filename[i]; i++ ) {
	        tu_strnzcpy ( &index_fname[length], http_index_filename[i],
			255-length );
	        o2f = tf_open ( index_fname, "r", buffer );
		if ( o2f ) break;
	    }
	    mfd = 0;
	    if ( !o2f ) {
		/* May be top level, retry with mfd */
		tu_strnzcpy ( &index_fname[length], "000000/", 255-length );
		length = tu_strlen ( index_fname );
		for ( i = 0; !o2f && http_index_filename[i]; i++ ) {
		     tu_strnzcpy ( &index_fname[length], 
			http_index_filename[i], 255-length );
	             o2f = tf_open ( index_fname, "r", buffer );
		}
		if ( o2f ) mfd = 1;	/* using master file directory */
	    }
	    if ( o2f ) {
		/*
		 * Found welcome file, variable i has index for filename found.
		 */
		if ( http_default_host && (scb->request[2].l > 0) ) {
		    /*
		     * Full HTTP/1.0 request, send redirect.
		     */
		    int local_port, remote_port, rem_addr;
		    stsmsg = buffer;
		    tu_strcpy ( stsmsg, 
			"302 Found index file\r\nLocation: http://" );
		    tu_strcpy ( &stsmsg[39], http_default_host );

		    ts_tcp_info ( &local_port, &remote_port, 
				(unsigned int *) &rem_addr );
		    if ( local_port != 80 ) {
			tu_strcpy ( &stsmsg[tu_strlen(stsmsg)], ":" );
			tu_strint ( local_port, &stsmsg[tu_strlen(stsmsg)] );
		    }
		    tu_strcpy ( &stsmsg[tu_strlen(stsmsg)], req_ident );
		    if ( mfd ) {
		        tu_strcpy ( &stsmsg[tu_strlen(stsmsg)], "000000/" );
		    }
		    tu_strcpy ( &stsmsg[tu_strlen(stsmsg)], 
				http_index_filename[i] );
		    tf_close ( o2f );
		} else {
		    /*
		     * Replace directory lookup with real file.
		     */
		    stsmsg = "200 Sending index file";
		    tf_close ( of );
		    of = o2f;
		    text_mode = 1;
		    suffix = ".HTML"; rep = "text/html"; 
		    encoding = "8bit"; enc_len = 4;
		}
	    } else if ( http_dir_access ) {
		/*
		 * No welcome file and browsing restricted.
		 */
		if ( http_dir_access == 1 ) {
	            tu_strnzcpy ( &index_fname[length-7], http_dir_access_file,
			255-(length-7) );
	            o2f = tf_open ( index_fname, "r", buffer );
		}
		if ( o2f ) {
		    tf_close ( o2f );	/* Access allowed, cleanup */
		} else {
		    stsmsg = "403 Directory browse disabled";
		    tf_close ( of );
		    status = http_send_error ( scb,
			"403 Directory browse disabled",
			"Directory not browsable.", buffer );
		    return status;
		}
	    }
	}
    } else {
	stsmsg = "200 Sending document";
	of = tf_open ( ident, "r", buffer );
    }
    /*
     * If name has no suffix (not even trailing '.') and open failed,
     * Check for .dir file and redirect.
     */
    if ( !of && (*suffix == '\0') && (http_dir_access < 2) ) {
	int fl;
	tu_strnzcpy ( fixup, ident, sizeof(fixup)-8 );
	fl = tu_strlen ( fixup );
	tu_strcpy ( &fixup[fl], ".dir.1");
	of = tf_open ( fixup, "r", buffer );
	if ( of && http_default_host ) {
	    /* Build redirect in buffer */
	    int local_port, remote_port, rem_addr;
	    stsmsg = buffer;
	    tu_strcpy ( stsmsg, 
			"302 Found directory file\r\nLocation: http://" );
	    tu_strcpy ( &stsmsg[43], http_default_host );

	    ts_tcp_info ( &local_port, &remote_port, 
			(unsigned int *) &rem_addr );
	    if ( local_port != 80 ) {
		tu_strcpy ( &stsmsg[tu_strlen(stsmsg)], ":" );
		tu_strint ( local_port, &stsmsg[tu_strlen(stsmsg)] );
	    }
	    tu_strcpy ( &stsmsg[tu_strlen(stsmsg)], req_ident );
	    tu_strcpy ( &stsmsg[tu_strlen(stsmsg)], "/" );
	    suffix = "/";
	}
    }
    /*
     * Check ownership of file if UIC specified, file must either be owned by
     * same UIC or a resource identifier.
     */
    if ( of && scb->acc->uic && (*suffix != '/') ) {
	unsigned int uic, cdate, mdate;
	int size;
	status = tf_header_info ( of, &size, &uic, &cdate, &mdate );
	if ( (status < 0)  || 
		( !(uic&0x80000000) && (uic != scb->acc->uic) ) ) {
	    tf_close ( of );
	    of = (void *) 0;
	    tu_strcpy ( buffer, "not owner." );
	}
    }
    if ( of ) {
	/*
	 * File opened and cleared for takeoff, format header line.
	 */
	http_add_response ( scb->rsphdr, stsmsg, 0 );
        http_add_response (  scb->rsphdr, "MIME-version: 1.0", 0 );
	tu_strcpy ( hdrline, "Server: OSU/" );
	tu_strcpy ( &hdrline[12], http_server_version );
	http_add_response ( scb->rsphdr, hdrline, 0 );
	tu_strcpy ( hdrline, "Content-type: " );
	tu_strcpy ( &hdrline[14], rep );
	http_add_response ( scb->rsphdr, hdrline, 0 );
	tu_strcpy ( hdrline, "Content-transfer-encoding: " );
	tu_strnzcpy ( &hdrline[27], encoding, enc_len );
	http_add_response ( scb->rsphdr, hdrline, 0 );
	if ( encoding[enc_len] == '/' ) {
	    tu_strcpy ( hdrline, "Content-encoding: " );
	    tu_strcpy ( &hdrline[18], &encoding[enc_len+1] );
	    http_add_response ( scb->rsphdr, hdrline, 0 );
	}
	if ( *suffix != '/' ) {
	    unsigned int uic, cdate, mdate;
	    int size;
	    /*
	     * Not a directory, add last-modified time and content-length.
	     */
	    status = tf_header_info ( of, &size, &uic, &cdate, &mdate );
	    if ( (status >= 0)  ) {
		if ( !text_mode ) {
		    tu_strcpy ( hdrline, "Content-length: " );
		    tu_strint ( size, &hdrline[16] );
		    http_add_response ( scb->rsphdr, hdrline, 0 );
		}
		tu_strcpy ( hdrline, "Last-Modified: " );
		tf_format_time ( mdate, &hdrline[15] );
		http_add_response ( scb->rsphdr, hdrline, 0 );
	        /*
	         * If request has if-modified-header, check time reset
	         * response buffer if file not updated.
	         */
	        if ( since_time && (since_time >= mdate) ) {
		    char *s;
		    for ( size = 0, s = scb->rsphdr->s; 
			s[size] && (s[size] != ' '); size++ );
		    scb->rsphdr->l = size+1;	/* Reset size to HTTP/1.0 */
		    http_add_response ( scb->rsphdr,
			"304 Document not modified", 0  );
		    http_add_response ( scb->rsphdr, hdrline, 0 );
		    header_only = 1;	/* skip sending data */
		}
	    }
	}
	status = http_send_response_header ( scb->cnx, scb->rsphdr );
	if ( ((status&1) == 1) && !header_only ) {
	    int size, used;
	    /*
	     * Send data, either binary, text, or directory.
	     */
	    if ( *suffix == '/' ) {
		/*
		 * Get and format directory lines (only if not re-directed).
		 */
		if ( tu_strncmp ( stsmsg, "302", 3 ) != 0 )
		    status = dir_to_html ( scb->cnx, of, buffer, bufsize, 
			&scb->data_bytes );
	    } else if ( text_mode ) {
		/*
		 * Get text lines and transmit.  Buffer multiple lines / write.
		 */
		for ( used = 0, size = 1; size > 0; ) {	
		    size = tf_getline(of, &buffer[used], bufsize-used, 2000 );
		    scb->data_bytes += size;
		    used += size;
		    if ( bufsize - used < 100 ) {
		        status = ts_tcp_write ( scb->cnx, buffer, used );
		        if ( (status&1) == 0 ) break;
			used = 0;
		    }
		}
		if ( used > 0 ) status = ts_tcp_write (scb->cnx, buffer, used);
	    } else {
		/*
		 * Read binary data.
		 */
		for ( ; 0 < ( size = tf_read(of,buffer,bufsize) );
			scb->data_bytes += size ) {
		    status = ts_tcp_write ( scb->cnx, buffer, size );
		    if ( (status&1) == 0 ) break;
		}
	    }
	    if ( ((status&1) == 1) && (scb->data_bytes < bufsize) ) {
		/*
		 * Save small documents in memory.
		 */
		if ( scb->acc->cache_allowed ) http_put_doc_cache 
			( ident, rep, encoding, buffer, scb->data_bytes );
	    }
	}
	/*
	 * Close to deallocate file access structure.
	 */
	tf_close ( of );
    } else {
	/* Open error, tf_open puts formatted error in buffer array */

	status = http_send_error ( scb,
		"404 error opening file in request", buffer );
    }
    return status;
}
/****************************************************************************/
/* Scan request header lines for fields matching requested field name
 * and append fields to single comma-separated list.  Note that result_length
 * points to an in/out variable. Field argument must be all lower case,
 * matches are done in case-insensitive fashion.
 */
static int extract_header_field ( char *field, string *request,
	char *buffer, int *result_length )
{
    int i, state, length, j, req_len, line_matched, tlog_putlog();
    char *p, *q, c;
    /*
     * Scan header lines (3+) in request for matching fields.
	    tlog_putlog(997,"extracting header info.!/");
     */
    length = *result_length;		/* output length */
    line_matched = 0;
    for ( i = 3; (req_len = request[i].l) > 0; i++ ) {
	/*
	 * If line is not continuation of previous line, test field label
	 * for match.
	 */
	p = request[i].s;
	if ( !isspace(*p) ) {
	    line_matched = 0;
	    for ( j = 0; (j < req_len) && (p[j] != ':'); j++ ) {
    	    	c = _tolower(p[j]);
	        if ( c != field[j] ) break;	/* no match */
	    }
	    if ( (j < req_len) && (p[j] == ':') ) line_matched = 1;
	}
	if ( !line_matched ) continue;
	/*
	 * Line matched, scan for field values and append to buffer.
	 */
	if ( length > 0 ) buffer[length++] = ',';
	for ( state = 0, j++; j < req_len; j++ ) {
	    if ( state == 0 ) {
	        if ( !isspace ( p[j] ) ) { 
		    buffer[length] = p[j]; _tolower(buffer[length]);
		    length++;
		    if ( p[j] == '"' ) state = 1;
		}
	    } else {
		/* Inside quoted string */
		buffer[length++] = p[j];
		if ( p[j] == '"' ) state = 0;
	    }
	}
    }
    *result_length = length;
    return 1;
}
/****************************************************************************/
/*  Read directory information and format into HTML document.
 */
static int dir_to_html ( void *ctx, void *of, char *buffer, int bufsize,
	int *data_bytes )
{
    int length, i, count, fmtlen, tot_len, status;
    struct tu_textbuf buf;
    char element[400];
    /*
     * Initialize textbuf structure to use buffer and add header text.
     */
    buf.l = 0;
    buf.s = buffer;
    buf.size = bufsize;

    tu_add_text ( &buf, 
	"<HTML><HEAD><TITLE>Directory listing</TITLE></HEAD>", 80 );
    tu_add_text ( &buf, "<BODY>Files:<DIR>\r\n", 80 );
    /*
     * Format list item for every directory entry.
     */
    count = 0;
    while ( (tot_len = tf_read(of, element, sizeof(element)) ) > 0 ) {
	for ( i = 0; i < tot_len; i += length + 1 ) {
	    /*
	     * Make sure we have enough room to construct element.
	     */
	    fmtlen = length = tu_strlen ( &element[i] );
	    if ( length > 4 ) {
		/* Convert directory files to / syntax */
		if ( tu_strncmp ( ".DIR", &element[i+length-4], 5 ) == 0 ) {
		    tu_strcpy ( &element[i+length-4], "/" );
		    fmtlen = length - 3;
		}
	    }
	    if ( buf.l + (2*fmtlen) + 22 > bufsize ) {
	        if ( buf.l > 0 ) status = ts_tcp_write ( ctx, buf.s, buf.l );
	        else status = 1;
	        if ( (status &1) == 0 ) return status;	/* error */
	        *data_bytes += buf.l;			/* Update stats */
	        buf.l = 0;
	    }
	    /*
	     * Append element to buffer.
	     */
	    count++;
	    tu_add_text ( &buf, "<LI><A HREF=\"", 14 );
	    tu_add_text ( &buf, &element[i], fmtlen );
	    tu_add_text ( &buf, "\">", 2 );
	    tu_add_text ( &buf, &element[i], fmtlen );
	    tu_add_text ( &buf, "</A>\r\n", 6 );
	}
    }
    /*
     * Add trailer and flush.
     */
    if ( buf.l + 80 > bufsize ) {
	if ( buf.l > 0 ) status = ts_tcp_write ( ctx, buf.s, buf.l );
	else status = 1;
	if ( (status &1) == 0 ) return status;	/* error */
	*data_bytes += buf.l;			/* Update stats */
	buf.l = 0;
    }
    tu_strint ( count, element );
    tu_add_text ( &buf,"\r\n</DIR>\r\nNumber of files in directory: ", 40 );
    tu_add_text ( &buf, element, tu_strlen(element) );
    tu_add_text ( &buf, "\r\n</BODY></HTML>\r\n",19  );
    status = ts_tcp_write ( ctx, buf.s, buf.l );
    *data_bytes += buf.l;

    return status;
}
