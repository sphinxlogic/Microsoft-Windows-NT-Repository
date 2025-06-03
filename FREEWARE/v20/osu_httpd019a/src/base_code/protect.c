/*
 * This module handles server access protection operations.  The cache
 * access inhibiting falls under this category.
 *
 *   int http_set_cache_inhibit ( int port );
 *
 *   int http_check_protection ( session_ctx scb, char *ident, string *iobuf );
 *
 *	input:
 *	    scb->ac	Protection info for ident:
 *			    owner UIC	(if /~username)
 *			    setup file (from rule file protect/defprot) rule.
 *
 *	    ident	Ident (filename) to access, after translation of
 *			URL according to rule file rules.
 *
 *	    scb->request Request and headers received from client.  This will
 *			be scanned for authorization lines.
 *
 *	modified:
 *	    iobuf	Scratch buffer for reading of setup file.
 *
 *	    scb->rsphdr	On error, will be loaded with HTTP error response,
 *			including authenicatation parameters.  If access
 *		 	check succeeds, rsphdr will not be modified.
 *		
 *	   ret. value:	0	Access is denied.
 *			1	Access is allowed.
 *
 *	int http_protect_fail ( string *request, tu_text rsphdr );
 *
 *  int http_check_cache_access ( access_info acc );
 *
 * Author:	David Jones
 * Revised:	9-JUN-1994		enhanced parsing, accept getmask.
 * Revised:	24-JUL-1994		Support protection by hostname.
 * Revised:	 8-FEB-1994		Extra diagnositice info in log output.
 * Revised:	11-MAY-1995		Fixup for OSF version (removes level2).
 */
#include <stdio.h>
#include "session.h"
#include "file_access.h"
#include "tserver_tcp.h"

struct ctx { void * f;
	int used, filled, state, bufsize; 
	char *buf; };

static int cache_inhibit_port = 0;
int http_log_level;			/* global variable */
int tlog_putlog ( int level, char *ctlstr, ... );
int http_dns_enable;
int http_get_doc_cache(), http_put_doc_cache(), http_add_response();
#ifdef VMS
int http_authenticate();
#else
#define http_authenticate(a,b,c,d) 0
#endif
/*****************************************************************************/
/* Set local port used to inhibit caching, returning previous value.
 */
int http_set_cache_inhibit ( int port )
{ int old;
   old = cache_inhibit_port;
   cache_inhibit_port = port;
   return old;
}
/*****************************************************************************/
/* Determine if cache access is allowed. */
int http_check_cache_access ( access_info acc )
{
    if ( cache_inhibit_port ) {
	/*
	 * Get connection information and inhibit cache if the local port
	 * is equal to cache inhibit port number.
	 */
	unsigned int remote_address;
	int local_port, remote_port, status;

        status = ts_tcp_info ( &local_port, &remote_port, &remote_address );
        acc->cache_allowed = (local_port != cache_inhibit_port ) ? 1 : 0;
        if ( http_log_level > 5 )
	    tlog_putlog ( 4, "port !SL cache_allowed flag: !SL!/", local_port,
		acc->cache_allowed );
    } else acc->cache_allowed = 1;
    return 1;
}
/*****************************************************************************/
/* Read next line from input file.
 */
static int read_line ( struct ctx *inp, char *line, int maxlen )
{
    int length;
    char octet;
    length = 0;
    do {
	if ( inp->used >= inp->filled ) {
	    inp->filled = inp->used = 0;
	    if ( ! inp->f ) return length;	/* end of file */

	    inp->filled = tf_read ( inp->f, inp->buf, inp->bufsize );
	    if ( inp->filled <= 0 ) {
		tf_close ( inp->f ); inp->f = (void *) 0;
		return length;
	    }
	}
	octet = inp->buf[inp->used++];
	if ( length < maxlen ) line[length++] = octet;
    } while ( octet != '\n' );
    return length;
}
/*************************************************************************/
/* Read setup file and test current information against rules in file. */

static int scan_setup_file ( struct ctx *inp, unsigned char *remote_address )
{
    int length, multi_line, rhost_len;
    char *type, *value, *p, line[500];
    char remote_host[256];
    /*
     * process file line by line.
     */
    rhost_len = -1;
    multi_line = 0;
    while ( 0 < (length = read_line ( inp, line, sizeof(line)-1) ) ) {
	/*
	 * Parse line into type field and value field.
	 */
	line[length] = '\0';
	if ( length > 0 ) if ( line[length-1] == '\n' ) line[length-1] = '\0';
	p = line;
	type = value = "";
	for ( type = line; *type && ((*type==' ') || (*type=='\t')); type++);
	if ( (*type == '!') || (*type == '#') ) continue;
        if ( !multi_line ) {
	    /*
	     * Not continuation, get line type.
	     */
	    for ( p = type; *p && ((*p != ' ') && (*p != '\t')); p++ );
	    *p++ = '\0';
	    tu_strupcase ( type, type );
	    if ( ( tu_strncmp ( type, "GETMASK", 8 ) !=0 ) &&
		(tu_strncmp ( type, "MASKGROUP", 10 ) != 0) ) continue;
        }
	/*
	 * parse out values.
	 */
	while ( p < &line[length] ) {
	    /* 
	     * Skip to start of field, ignoring commas 
	     */
	    for ( value = p; *value && 
		((*value==' ') || (*value=='\t') || (*value==',')); value++)
			if ( *value == ',' ) multi_line = 1;;
	    if ( *value ) multi_line = 0;
	    /*
	     * Find of end of field and terminate 
	     */
	    for ( p=value; *p && 
		((*p != ',') && (*p != ' ') && (*p != '\t')); p++ );
	    if ( *p == ',' ) multi_line = 1;
	    *p++ = '\0';
	    /*
	     * Ignore values with usernames.
	     */
            if ( http_log_level > 6 )
	        tlog_putlog(7,"groupdef from prot file: '!AZ'!/", value );
	    if ( *value != '@' ) continue;
	    if ( (value[1] >= '0') && (value[1] <= '9') ){
	        /*
	         * See if host address in value matches remote numeric address.
	         */
	        char number[16], *d;
	        int i, j, len;
	        for ( d = &value[1], i = 0; i < 4; i++ ) {
		    if ( (*d == '*') && ((d[1]=='.')||(d[1]=='\0')) ) {
		        len = 2;
		    } else {
		        tu_strint ( remote_address[i], number );
		        len = tu_strlen ( number );
		        number[len++] = ( i < 3 ) ? '.' : '\0';
			number[len] = '\0';
		        if ( tu_strncmp ( number, d, len ) != 0 ) {
			    /* Mismatch. */
			    break;
		        }
		    }
		    d += len;
	        }
	        if ( i == 4 ) return 1;	/* we have a match */
	    }
	    else if ( http_dns_enable ) {
		/*
		 * Parse and compare based on name.
		 */
		char *d;
	        int i, j, k, len;
		if ( rhost_len < 0 ) {
		    tu_strnzcpy ( remote_host, ts_tcp_remote_host(), 255 );
		    tu_strupcase ( remote_host, remote_host );
		    rhost_len = tu_strlen ( remote_host );
		    remote_host[rhost_len] = '.';
		}
		/* Break hostname into separate labels */
		tu_strupcase ( value, value );
		if ( http_log_level > 6 ) {
		    remote_host[rhost_len+1] = '\0';
        	    if ( http_log_level > 6 )
		       tlog_putlog(7,"Comparing host '!AZ' with '!AZ!/", 
				remote_host, value );
		}
		d = &value[1];
		for (j=i=0; i <= rhost_len; i++ ) if (remote_host[i]=='.') {
		    /* String goes from j to i, ending in period */
		    for ( k=0; d[k] && (d[k]!='.'); k++ );
		    if ( http_log_level > 6 ) tlog_putlog(7,
			"remote_host[!SL..!SL] = '!AF' and d = '!AF'!/", j, i, 
			i-j, &remote_host[j], k, d);
		    len = i - j;
		    if ( (k != 1) || (d[0] != '*') ) {

		        if ( k != len) break;	/* mismatch */
		        if ( tu_strncmp(d,&remote_host[j],len) ) break;
		    }
		    d += k; if ( *d ) d++;
		    j = i+1;
		}
		/* We pass if If all labels matched no more value; */
		if ( (i > rhost_len) && !*d ) return 1;
	    }
	}
    }
    /*
     * Nothing matched, fail.
     */
    return 0;
}
/***********************************************************************/

int http_check_protection ( 
	session_ctx scb,			/* session control block */
	char *ident,				/* Ident to access */
	string *iobuf)				/* scratch space for I/O */
{
    int status, local_port, remote_port, length, left;
    unsigned int remote_address;
    struct ctx inp;
    access_info acc;		/* local copy of scb->acc */
    /*
     * Get address of client and set default value whether caching permitted.
     */
    status = ts_tcp_info ( &local_port, &remote_port, &remote_address );
    acc = scb->acc;
    acc->cache_allowed = (local_port != cache_inhibit_port ) ? 1 : 0;
    if ( http_log_level > 5 )
	tlog_putlog ( 4, "port !SL cache_allowed flag: !SL prot file: '!AZ'!/", 
		local_port, acc->cache_allowed, acc->prot_file );
    /*
     * Check whether setup file is level 2 protection.
     */
    if ( acc->prot_file[0] == '+' ) {
	status = http_authenticate ( acc, ident, scb->request, scb->rsphdr );
	return status;
    }
    /*
     * See if setup_file in cache, otherwise read it.  Use unique encoding
     * to ensure that we don't retrieve served file.
     */
    inp.f = (void *) 0;
    inp.used = inp.filled = inp.state = 0;
    inp.bufsize = iobuf->l;
    inp.buf = iobuf->s;			/* data area for I/O */

    if ( (!acc->cache_allowed) ||
	 (http_get_doc_cache ( acc->prot_file, "text/plain", "ACCESS", 
		inp.buf, inp.bufsize, &inp.filled ) == 0) ) {

	inp.f = tf_open ( acc->prot_file, "r", inp.buf );
	if ( !inp.f ) return 0;		/* error reading file */
	
	for ( left=inp.bufsize; left > 0; left = left - length ) {
	    length = tf_read ( inp.f, &inp.buf[inp.bufsize-left], left );
	    if ( length <= 0 ) break;
	}
	inp.filled = inp.bufsize - left;
	if ( left > 0 ) {
	    if ( acc->cache_allowed )
		http_put_doc_cache ( acc->prot_file, "text/plain", "ACCESS", 
			inp.buf, inp.filled );
	    tf_close ( inp.f );
	    inp.f = (void *) 0;	/* mark file closed */
	}
    }
    /*
     * Parse setup file.  While scanning validate remote_address against
     * group mask.
     */
    status = scan_setup_file ( &inp, (unsigned char *) &remote_address );
    if ( inp.f ) tf_close ( inp.f );
    if ( (status &1) == 0 ) {
	/*
	 * Parse failed, for level 1 protection just return forbidded status.
	 */
	http_add_response ( scb->rsphdr, "403 Forbidden", 1 );
	return 0;
    }
    return status;
}
/****************************************************************************/
int http_protect_fail ( string *request, tu_text rsphdr )
{
    int status, http_add_response();
    status = http_add_response ( rsphdr, "401 Unauthorized", 0 );
    status = http_add_response ( rsphdr, 
		"WWW-authenticate: Basic realm=\"VMS password\"", 1 );
    return status;
}
