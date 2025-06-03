/*
 * Thread-based DECnet script server for use with HTTP server.
 *
 * Command-line format:
 *
 *	$ targate err-file rule-file
 *
 * args:
 *    err-file	    Name of log file to create, connections and query strings
 *		    are written to the log file.
 *
 *    rule-file	    Name of rules file.  The rules file maps suffixes to
 *                  conntent types.
 *
 *    taskname-num Object name or object number to accept connects on.
 *
 * Environment variables:
 *    HTGT_CLIENT_LIMIT		If defined, limits maximum number of concurrent
 *				TCP connections the server is to support,
 *				default is 16.  At startup, current process
 *				quotas are checked and client limit will be
 *				reduced if needed to match the available
 *				resources.  See file client_limit.c for
 *				details.
 *
 *    HTGT_MIN_AVAILABLE	If defined, sets mininum number of clients free
 *				clients that must be available for the request
 *				to be processed.  If below this number, an
 *				error is returned.
 *
 *    HTGT_LOG_LEVEL		If defined, sets logging level (default=1):
 *				  common Use standard log file format.
 *				    0	Report errors, config file info.
 *				    1	Report connect/final stat info.
 *				    2	Report Identifier re-mapping
 *				    3	Report request/response byte counts.
 *
 *
 *    HTGT_REENTRANT_C_RTL	If defined, sets reentrancy for C runtime:
 *				    0	Library is not reentrant, global locks
 *					will be used to serialize RTL calls.
 *				    1	Library is reentrant, synchronization
 *					for library set to C$C_MULTITHREAD.
 *				If not defined, default value is 0.
 *
 *  Author:	David Jones
 *  Date:	 5-AUG-1994
 *  Revised:	11-OCT-1994	Include transfer encoding header.
 *  Revised:	29-NOV-1994
 *  Revised:	14-JAN-1995	Changed encoding.
 *  Revised:	14-JUN-1995	Support sub-directory lists.
 */
#include "pthread_np.h"
#include <unixlib.h>
#include <time.h>
#include <stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <prvdef.h>
#include <descrip.h>
#include <lnmdef.h>
#include "tutil.h"		/* threaded utility routines */
#include "tserver_decnet.h"
#include "file_access.h"
#include "decnet_searchlist.h"
#include "tarscan.h"

int tlog_putlog ( int level, char * ctlstr, ... );
int ts_declare_decnet_object(), ts_decnet_close(), http_read_rules();
int ts_decnet_write(), ts_decnet_read();

int tar_extract ( void *ctx, struct ncbdef *ncb, int ndx, int available );
int tlog_init(), tlog_initlog();

#define RESPONSE_LINE_LIMIT 100
#define HTGT_DEFAULT_CLIENT_LIMIT 16
#define DEFAULT_CACHE_SIZE 32
#define DEFAULT_LOG_LEVEL 1
typedef struct { int l; char *s; } string;
/*
 * Global (program-wide) variables.
 */
int http_log_level;			/* Logging detail control */
static int min_available;
static int send_module(), send_catalogue();
/**************************************************************************/
/* Main program. */
int main ( int argc, char **argv )
{
    int status, i, port_num, client_limit, exit_status, http_port;
    int nocache_port, cache_size, length, acmode, server_mode, SYS$TRNLNM();
    char *envval, *log_file_name, *rule_file_name, taskname[256];
    long priv_mask[2], prev_priv[2], sys$setprv(), is_sysgrp();
    pthread_t http_thread, http_thread2;
    pthread_attr_t client_attr;
    struct { short length, code; char *buffer; int *retlen; } item[3];
    $DESCRIPTOR(table_name,"LNM$FILE_DEV");
    $DESCRIPTOR(object_logical,"WWW_TARSERV_OBJECT");
    /*
     * Disable sysprv and sysnam privilege.
     */
    priv_mask[0] = PRV$M_SYSPRV|PRV$M_SYSNAM; priv_mask[1] = 0;
    status = sys$setprv ( 0, priv_mask, 0, prev_priv );
    printf("disable SYSPRV status: %d, prev set: %d\n", status,
	(prev_priv[0]&PRV$M_SYSPRV) ? 1 : 0 );
    if ( is_sysgrp() ) printf 
	( "Warning, system group id gives implicit SYSPRV\n" );
    /*
     * Validate command line arguments, defaulting if needed.
     */
    log_file_name = "sys$output";
    rule_file_name = "";
    strcpy ( taskname, "HTGATE" );
    if ( argc > 1 ) log_file_name = argv[1];
    if ( argc > 2 ) rule_file_name = argv[2];
    /*
     * Init log file and threaded file access module and read rules file.
     */
    http_log_level = DEFAULT_LOG_LEVEL;
    envval = getenv("HTGT_LOG_LEVEL");
    if ( envval ) {
	if ( (*envval == 'C') || (*envval == 'c') ) http_log_level = -1;
	else http_log_level = atoi ( envval );
    }

    status = tlog_init ( log_file_name );
    printf("Log file '%s' init status: %d\n\n", log_file_name, status );
    tlog_putlog ( 0,
	"DECThreads network object, version 1.2, 14-JUN-95, compiled !AZ !AZ!/", 
	__DATE__, __TIME__ );
    /*
     * Set up security, logical name defines list of trusted sources.
     */
    ts_set_access ( "WWW_TARSERV_ACCESS" );
    /*
     * Translate WWW_DIRSERV_OBJECT to determine operating mode.
     */
    acmode = 0;
    item[0].code = LNM$_STRING; item[0].length = 255;
    item[0].buffer = taskname; item[0].retlen = &length; length = 0;
    item[1].code = LNM$_ACMODE; item[1].length = sizeof(acmode);
    item[1].buffer = (char *) &acmode; item[1].retlen = (int *) 0;
    item[2].code = item[2].length = 0;

    status = SYS$TRNLNM ( 0, &table_name, &object_logical, 0, &item );
    tlog_putlog ( 0, "trnlnm status: !SL, acmode: !UB!/", status, acmode );
    if ( ((status&1) == 0) || (acmode > 1) ) {
	server_mode = 0;		/* not a server */
	strcpy ( taskname, "sys$net" );
    } else {
	taskname[length] = '\0';
	server_mode = 1;
    }

    status = dnetx_initialize ( "WWWEXEC" );		/* Init node search lists */
    if ( (status&1) == 0 ) exit ( status );
    status = tlog_initlog ( -1 , "tar_access.log" );
    status = http_read_rules ( rule_file_name );	/* Load configuration*/
    if ( (status&1) == 0 ) exit ( status );
    status = tlog_initlog ( -1 , "nl:" );
    tf_initialize("");

    min_available = 0;
    envval = getenv("HTGT_MIN_AVAILABLE");
    if ( envval ) min_available = atoi ( envval );
    envval = getenv("HTGT_REENTRANT_C_RTL");
    http_reentrant_c_rtl = envval ? atoi ( envval ) : 0;
#ifdef __DECC
    if ( http_reentrant_c_rtl ) {
	decc$set_reentrancy ( C$C_MULTITHREAD );
        tlog_putlog ( 0, "Set DECC library for multi-thread use!/" );
     }
#endif
    /*
     * Compute client limit.
     */
    if ( server_mode ) {
        client_limit = HTGT_DEFAULT_CLIENT_LIMIT;
        envval = getenv ( "HTGT_CLIENT_LIMIT" );
        if ( envval ) client_limit = atoi ( envval );
    } else client_limit = 1;
    cache_size = 0;
    tar_init ( client_limit );

    tlog_putlog ( 0,
	"!/Port: !SL, Client limit: !SL, logger level: !SL!/", http_port,
		client_limit, http_log_level );
    /*
     * Start listening on DECnet ports.  Temporarily re-enable privs.
     */
    ts_set_logging ( tlog_putlog );
    pthread_attr_create ( &client_attr );
    pthread_attr_setinheritsched ( &client_attr, PTHREAD_DEFAULT_SCHED );
    pthread_attr_setstacksize ( &client_attr, 62000 );
    status = sys$setprv ( 1, priv_mask, 0, 0 );

    status = ts_declare_decnet_object 
	( taskname, client_limit, &client_attr, &http_thread, tar_extract );
    (void) sys$setprv ( 0, priv_mask, 0, 0 );

    tlog_putlog(0,"Status of declare object (!AZ) = !SL!/", taskname, status);
    /*
     * Pre-load archives.
     */
    for ( i = 3; i < argc; i++ ) {
	char errmsg[256];
	void *tarf;
	tarf = tar_open ( argv[i], errmsg );
	if ( tarf ) tar_close ( tarf );
	else tlog_putlog ( 0, "Error pre-loading !AZ: !AZ", argv[i], errmsg );
    }
    /*
     * Wait for DECnet communication thread to rundown.
     */
    if ( (status&1) == 1 ) status = 
		pthread_join ( http_thread, (void *) &exit_status );
    else exit_status = status;
    tlog_putlog ( 0, "!/listener thread exit status: !SL!/", exit_status );
    if ( status == 0 ) return exit_status;
    else return status;
}
/***********************************************************************/
/* Main routine for handling htgt server connection.  This function is called 
 * as the thread init routine for tcp-ip server threads.
 */
int tar_extract ( void *ctx, 		/* TCP connection context */
	struct ncbdef *ncb,
	int ndx,			/* Thread index */
	int available )			/* # of contexts left on  free list */
{
    int length, status, tlog_flush(), ts_decnet_info(), i, k, tf_len, bd_len;
    string prologue[4];
    string request[128];
    char prolog_buf[1024];

    char exec_path[256], errmsg[256], tar_file[256];
    char taskname[256], cre_date[32], remote_node[64], remote_user[64];
    char *bp, *opcode, *method, *url, *protocol, *module, log_prefix[32];
    struct tu_streambuf inbound;
    struct tu_textbuf response;
    void *tarf;
    stat_t stat_buf;
    /*
     * Make prefix string for log entries so we can follow interleaved
     * entries in log file.  Log the connection with time.
     */
    ts_decnet_info ( taskname, remote_node, remote_user );
    tlog_putlog ( 3, "task: '!AZ', node: '!AZ', user: '!AZ'!/",
	taskname, remote_node, remote_user );
    LOCK_C_RTL
    sprintf ( log_prefix, "DNT-%s/%d", taskname, ndx );
    UNLOCK_C_RTL

    /*                   0      1        2        3
     * Read prologue (module, method, protocol, ident) sent by HTTP server.
     */
    for ( i = 0, bp = prolog_buf; i < 4; i++ ) {
	status = ts_decnet_read ( ctx, bp, 255, &length );
	if ( (status&1) == 1 ) {
	    prologue[i].l = length;
	    prologue[i].s = bp;
	    bp[length++] = '\0';	/* safety first, terminate string */
	    bp = &bp[length];
	} else {
	    tlog_putlog ( 0, "!AZ, Error reading prologue: !SL!/", log_prefix,
			status );
	    return status;
	}
    }
    /*
     * Get bin dir parameter from server exec file and use it as
     * prefix for tar archive file name.
     */
    status = ts_decnet_write ( ctx, "<DNETBINDIR>", 12 );
    status = ts_decnet_read ( ctx, tar_file, 255, &bd_len );
    if ( (status&1) == 0 ) return status;
    tf_len = bd_len;
    tar_file[tf_len] = '\0';		/* terminate string */

    tlog_putlog(3,"!AZ, tar archive dir: !AF, open: !SL!/", log_prefix, 
	tf_len, tar_file );
    if ( !tarf ) {
	tlog_putlog(0,"!AZ, !AZ!/", log_prefix, errmsg );
	return 44;
    }
    /*
     * Get 'exec' path portion of ident.
     */
    status = ts_decnet_write ( ctx, "<DNETPATH>", 10 );
    status = ts_decnet_read ( ctx, exec_path, 255, &length );
    if ( (status&1) == 0 ) return status;
    exec_path[length] = '\0';
    tlog_putlog(3,"!AZ, path: '!AZ' (!SL), URL: '!AZ'!/", log_prefix,
	exec_path, length, prologue[3].s );
    /*
     * Parse prologue[3] (ident) into 3 parts: 'exec' path, archive name, and
     * archive module name.
     */
    if ( (length < prologue[3].l) && ( length > 0) ) {
	int size;
	/*
	 * Scan for first field and append it to tar_file string, then
	 * update url pointer to skip it.
	 */
	module = &prologue[3].s[length];
	for ( k = 0; module[k]; k++ ) {
	    if ( module[k] == '/' ) { module[k++] = '\0'; break; }
	    if ( tf_len < 255 ) tar_file[tf_len++] = module[k];
	}
	tar_file[tf_len] = '\0';
	module = &module[k];
    } else {
	/*
	 * Null field passed, make error message.
	 */
	return 44;
    }
    tlog_putlog(3,"!AZ, archive: '!AZ', module: '!AZ'!/", 
		log_prefix,  tar_file, module );
    /*
     * Validate method.
     */
    if ( tu_strncmp ( "GET", prologue[1].s, 4 ) && 
		tu_strncmp("HEAD",prologue[1].s, 5) ) {
        status = ts_decnet_write ( ctx, "<DNETTEXT>", 10 );
        status = ts_decnet_write ( ctx, 
	/* 12345678901234567890123456789012345678901234567 8 9 0 123 */
	  "400 Bad method in request.", 26);
	status = ts_decnet_write ( ctx, "Unsupported method", 18 );
	status = ts_decnet_write ( ctx, "</DNETTEXT>", 11 );
	return 1;
    }
    /*
     * Attempt to open tar archive.
     */
    tarf = tar_open ( tar_file, errmsg );
    if ( !tarf ) {
	tlog_putlog ( 0, "archive open failure: !AZ!/", errmsg );
        status = ts_decnet_write ( ctx, "<DNETTEXT>", 10 );
        status = ts_decnet_write ( ctx, 
	/* 1234567890123456789012 3 456789012345678901234567 8 9 0 123 */
	  "404 document not found.", 23 );
	status = ts_decnet_write ( ctx, errmsg, tu_strlen(errmsg) );
	status = ts_decnet_write ( ctx, "</DNETTEXT>", 11 );
	return 1;
    }
    /*
     * Send module or catalogue.
     */
    status = ts_decnet_write ( ctx, "<DNETRAW>", 9 );
				/*       1234567890123456789012345678901234 */
    k = 0; if ( *module ) while ( module[k+1] ) k++;	/* find last char */
    if ( *module && module[k] != '/') {
	status = send_module ( ctx, tarf, module, prologue[2].s, log_prefix );
    } else {
	status = send_catalogue ( ctx, tarf, prologue[3].s, module,
			prologue[2].s, log_prefix );
    }
    status = tar_close ( tarf );

    status = ts_decnet_write ( ctx, "</DNETRAW>", 10 );
    status = ts_decnet_read ( ctx, prolog_buf, 255, &length );
    tlog_putlog ( 6, "!AZ, status of final read: !SL!/", log_prefix, status );
    status = ts_decnet_close ( ctx );
    if ( (status&1) == 0 )
        tlog_putlog ( 0, "!AZ, status of close !SL!/", log_prefix, status );
    tlog_flush();
    return status;
}
/********************************************************************/
/* Buffer output in chunks.
 */
struct out_stream { void *ctx; int used; char buf[1024]; };

static int put_text ( struct out_stream *stream, char *text )
{
    int used, status;
    char *buf;
    used = stream->used;
    for ( buf = &stream->buf[used]; *text; used++ ) {
	if ( used >= sizeof(stream->buf) ) {
	    status = ts_decnet_write ( stream->ctx, stream->buf, used );
	    if ( (status&1) == 0 ) return status;
	    used = 0;
	    buf = stream->buf;
	}
	*buf++ = *text++;
    }
    stream->used = used;
    return 1;
}
/*************************************************************************/
/*
 * Routines to convert between internal and external representations of
 * module names.  The external representation is caseless - characters will
 * be assumed to be lowercase unless escaped by a dollar sign.  A dollar sign
 * is encoded as $$.
 */
static char *encode_module_name ( char *x_module, char *module, int maxlen )
{
    int i, j;
    char cur_c;
    maxlen = maxlen - 1;
    for ( j = i = 0; i < maxlen; i++ ) {
	/*
	 * Get next character from input string.
	 */
	cur_c = module[j++];
	if ( ((cur_c >= 'A') && (cur_c <= 'Z')) || (cur_c == '$') ) {
	    if ( (i + 1) >= maxlen ) break;
	    x_module[i++] = '$';		/* insert flag character */
	}
	x_module[i] = cur_c;
    }
    x_module[i] = '\0';
    return x_module;
}
static char *decode_module_name ( char *module )
{
    int i, j;
    char *cp, temp[2];
    tu_strlowcase ( module, module );

    for ( i = j = 0; module[i]; i++ ) {
	if ( module[i] == '$' ) {
	    temp[0] = module[i+1]; temp[1] = '\0';
	    tu_strupcase ( temp, temp );
	    module[j++] = temp[0];
	    if ( module[i+1] ) i++;
	}
	else module[j++] = module[i];
    }
    module[j] = '\0';
    return module;
}
/*************************************************************************/
static int send_module ( void *ctx, void *tarf, char *module, 
	char *protocol, char *log_prefix )
{
    int size, i, status, length, j, enc_len, http_match_suffix();
    char *suffix, *rep, *encoding, *text, numbuf[16], errmsg[256];
    struct out_stream stream;
    /*
     * Attempt to lookup module.
     */
    decode_module_name ( module );
    stream.ctx = ctx;
    stream.used = 0;
    size = 0;
    status = tar_set_module ( tarf, module, &size, errmsg );
    if ( !status ) {
	if ( *protocol ) put_text ( &stream, "HTTP/1.0 " );
	put_text ( &stream, "404 Module not found '" );
	put_text ( &stream, protocol ); put_text ( &stream, "'\r\n" );
	if ( *protocol ) {
	    put_text ( &stream, "MIME-version: 1.0\r\n" );
	    put_text ( &stream, "Content-type: text/plain\r\n\r\n" );
	}
	put_text ( &stream, "Module not found in archive\r\n" );
        if ( stream.used > 0 )
            ts_decnet_write ( ctx, stream.buf, stream.used );
	return status;
    }
    /*
     * Determine content-type.
     */
    suffix = "";
    for ( i = 0; module[i]; i++ ) 
	if ( (module[i] == '.') | (module[i] == '/') ) suffix = &module[i];
    if ( !http_match_suffix ( suffix, "*/*", &rep, &encoding ) ) {
	rep = "text/plain";
	encoding = "8BIT"; enc_len = 4;
    } else {
	for (enc_len=0; encoding[enc_len] && (encoding[enc_len]!='/');
		enc_len++);
    }
    /*
     * Send status and mime header.
     */
    if ( *protocol ) {
	put_text ( &stream, "HTTP/1.0 200 Sending Document\r\n" );
	put_text ( &stream, "MIME-version: 1.0\r\n" );
	put_text ( &stream, "content-type: " );
	put_text ( &stream, rep );
	put_text ( &stream, "\r\ncontent-length: " );
	put_text ( &stream, tu_strint ( size, numbuf) );
	if ( encoding[enc_len] == '/' ) {
	    /*
	     * Encoding is special, includes content-encoding.
	     */
	    put_text ( &stream, "\r\ncontent-transfer-encoding: " );
	    tu_strnzcpy ( errmsg, encoding, 
		enc_len < sizeof(errmsg)-1 ? enc_len : sizeof(errmsg)-1 );

	    put_text ( &stream, errmsg );
	    put_text ( &stream, "\r\ncontent-encoding: " );
	    put_text ( &stream, &encoding[enc_len+1] );
	} else {
	    put_text ( &stream, "\r\ncontent-transfer-encoding: " );
	    put_text ( &stream, encoding );
	}
	put_text ( &stream, "\r\n\r\n" );
    } else {
	put_text ( &stream, "200 Sending Document\r\n" );
    }
    if ( stream.used > 0 ) {
        status = ts_decnet_write ( ctx, stream.buf, stream.used );
    }
    /*
     * Transfer data.
     */
    text = stream.buf;
    if ( status ) for ( ; tar_read ( tarf, text, 1024, &length ) > 0; ) {
	status = ts_decnet_write ( ctx, text, length );
	if ( (status&1) == 0 ) break;
	size = size - length;
    }
tlog_putlog(6,"Final text dump status: !SL, remaining bytes !SL!/", 
	status, size );
    return status;
}
/********************************************************************/
/*  Format tar file index as a hypertext document.
 */
static int send_catalogue ( void *ctx, void *tarf, char *archive_name, 
	char *subdir, char *protocol, char *log_prefix )
{
    int size, status, length, sdir_len, ts_tcp_stack_used();
    unsigned mtime;
    struct out_stream stream;
    char numbuf[16], timbuf[32], anchor_base[256], modname[500];
    /*
     * Send mime header.
     */
    stream.ctx = ctx;
    stream.used = 0;
    if ( *protocol ) {
	put_text ( &stream, "HTTP/1.0 200 Sending Catalogue\r\n" );
	put_text ( &stream, "MIME-version: 1.0\r\n" );
	put_text ( &stream, "content-type: text/html\r\n" );
	put_text ( &stream, "content-transfer-encoding: 8bit\r\n\r\n" );
    } else {
	put_text ( &stream, "200 sending document\r\n" );
    }
    /*
     * Construct base for setting anchor.
     */
    tu_strcpy ( anchor_base, "<A HREF=\"" );
    tu_strnzcpy ( &anchor_base[9], archive_name, 240 );
    tu_strcpy ( &anchor_base[tu_strlen(anchor_base)], "/" );
    /*
     * Send HTML header.
     */
    status = put_text ( &stream, 
	"<HTML><HEAD><TITLE>Tar file contents</TITLE></HEAD>\r\n" );
    if ( (status&1) == 0 ) return status;
    status = put_text ( &stream, "<BODY>\r\n<H1>" );
    status = put_text ( &stream, archive_name );
    sdir_len = tu_strlen ( subdir );
    if ( sdir_len > 0 ) {
	put_text ( &stream, "/" );
	put_text ( &stream, subdir );
    }
    status = put_text ( &stream, "</H1><BR>\r\n" );
    /*
     * Send module names inside anchor.
     */
    for ( ; tar_read_dir ( tarf, modname, &size, &mtime ) > 0; ) {
	char x_name[200];
	if ( sdir_len > 0 ) {
	     /*
	      * Skip directories that don't match.
	      */
	    if ( tu_strncmp ( modname, subdir, sdir_len ) ) continue;
	}
	encode_module_name ( x_name, modname, sizeof(x_name) );
	put_text ( &stream, anchor_base );
	put_text ( &stream, x_name );
	put_text ( &stream,"\">" );
	put_text ( &stream, modname );
	put_text ( &stream, "</A>, ");
	put_text ( &stream, tu_strint(size,numbuf) );
	put_text ( &stream, " bytes, " );
	pthread_lock_global_np();
	tu_strncpy ( timbuf, ctime ( (unsigned long *) &mtime ), 31 );
	pthread_unlock_global_np();
	put_text ( &stream, timbuf );
	put_text ( &stream, "<BR>\r\n" );
    }
    /*
     * Close HTML and flush.
     */
    put_text ( &stream, "</BODY></HTML>\r\n" );
    if ( stream.used > 0 ) 
	status = ts_decnet_write ( ctx, stream.buf, stream.used );
    return status;
}
