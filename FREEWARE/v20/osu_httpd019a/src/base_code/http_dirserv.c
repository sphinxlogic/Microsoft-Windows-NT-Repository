/*
 * Thread-based DECnet script server for use with HTTP server.  Provide
 * extended directory listing.  Note that this is a scriptserver program,
 * not a WWWEXEC-based script.
 *
 * The program reads a configuration file to determine how to present
 * the directory.  The file format is that same as for the HTTP server
 * but the rule set is different:
 *
 *     Include file-name		# Include specified file.
 *     TraceLevel level [file]		# set logging detail level
 *     Welcome file-name		# add directory index to search list.
 *     DirAccess [ON|OFF|SELECTIVE [control-file [OVERRIDE]]]
 *					# controls file listing.
 *     DirReadme [OFF|TOP|BOTTOM] [readme-file]
 *					# controls inclusion of README files.
 *     DirShowDate [ON|OFF]		# Include last modify date.
 *     DirShowSize [ON|OFF]		# Include file size.
 *     DirShowBytes [ON|OFF]		# Report size in bytes rather than Kb.
 *     DirShowHidden [ON|OFF]		# Supress listing ".ext" files.
 *
 * The name of the configuration file is supplied by the HTTP server 
 * configuration (see below).
 *
 * Command-line format:
 *
 *	$ dirserv err-file
 *
 * args:
 *    err-file	    Name of log file to create, connections and query strings
 *		    are written to the log file.
 *
 * Logical names:
 *    WWW_DIRSERV_OBJECT	If defined as an exec mode logical, indicates
 *				that server should become a persitent network
 *				object using the equivalence name as taskname.
 *				If not defined, program will translate sys$net
 *				and service a single request.
 *
 *    WWW_DIRSERV_ACCESS	List of node/username pairs that are allowed
 *				to access this object.  If missing, no access
 *				checks are performed.
 *
 * Environment variables:
 *    HTDS_CLIENT_LIMIT		If defined, limits maximum number of concurrent
 *				TCP connections the server is to support,
 *				default is 16.
 *
 *    HTDS_MIN_AVAILABLE	If defined, sets mininum number of clients free
 *				clients that must be available for the request
 *				to be processed.  If below this number, an
 *				error is returned.
 *
 *    HTDS_LOG_LEVEL		If defined, sets logging level (default=1):
 *				  common Use standard log file format.
 *				    0	Report errors, config file info.
 *				    1	Report connect/final stat info.
 *				    2	Report Identifier re-mapping
 *				    3	Report request/response byte counts.
 *
 *
 *    HTDS_REENTRANT_C_RTL	If defined, sets reentrancy for C runtime:
 *				    0	Library is not reentrant, global locks
 *					will be used to serialize RTL calls.
 *				    1	Library is reentrant, synchronization
 *					for library set to C$C_MULTITHREAD.
 *				If not defined, default value is 0.
 *
 * HTTP server configuration:
 *     To configure this program as the server's directory presentation
 *     script, add a presentation rule with the following form:
 *
 *		presentation text/file-directory node::"0=WWWDIR"config-file
 *
 *  Author:	David Jones
 *  Date:	 1-DEC-1994
 *  Revised:    16-DEC-1994		Support HEAD command (req. by spec).
 *  Revised:    17-DEC-1994		Added remote user check.
 *  Revised:	21-FEB-1995		Fix SYSNAM report and cleanly close
 *					connections.
 *  Revised:    16-MAR-1995		Fix bug introduced by 21-feb fix.
 *  Revised:	31-MAR-1995		Work around Netscape bug.
 */
#include "pthread_np.h"
#include <descrip.h>
#include <lnmdef.h>
#include <time.h>
#include <stat.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <prvdef.h>
#include "tutil.h"		/* threaded utility routines */
#include "file_access.h"
#include "tserver_decnet.h"	/* Network I/O */
#include "dirserv_options.h"	/* dir_opt typdef */
#define CONDITIONAL_YIELD if ( !http_reentrant_c_rtl ) pthread_yield();

int tlog_putlog ( int level, char * ctlstr, ... );
int tlog_init(), tlog_initlog();
int htds_read_rules(), htds_get_rules();

int dir_request ( void *ctx, struct ncbdef *ncb, int ndx, int available );

#define RESPONSE_LINE_LIMIT 100
#define HTDS_DEFAULT_CLIENT_LIMIT 16
#define DEFAULT_CACHE_SIZE 32
#define DEFAULT_LOG_LEVEL 1
typedef struct { int l; char *s; } string;
struct out_stream { void *ctx; int used; char buf[1024]; };
/*
 * Global (program-wide) variables.
 */
int http_log_level;			/* Logging detail control */
char **http_index_filename;		/* List of welcome files */

static int min_available;
static int send_module(), send_catalogue();
static char conf_file[256];
static int list_files ( struct out_stream *out, void *dirf, dir_opt *opt,
	char *dirbuf, int bufsize, char *full_name, char *tail );
/**************************************************************************/
/* Main program. */
int main ( int argc, char **argv )
{
    int status, i, port_num, client_limit, exit_status, http_port;
    int nocache_port, cache_size, server_mode, acmode, length, SYS$TRNLNM();
    char *envval, *log_file_name, *rule_file_name, taskname[256];
    long priv_mask[2], prev_priv[2], sys$setprv(), is_sysgrp();
    pthread_t http_thread, http_thread2;
    pthread_attr_t client_attr;
    struct { short length, code; char *buffer; int *retlen; } item[3];
    $DESCRIPTOR(table_name,"LNM$FILE_DEV");
    $DESCRIPTOR(object_logical,"WWW_DIRSERV_OBJECT");
    /*
     * Disable sysnam privilege.
     */
    priv_mask[0] = PRV$M_SYSNAM; priv_mask[1] = 0;
    status = sys$setprv ( 0, priv_mask, 0, prev_priv );
    printf("disable SYSNAM status: %d, prev set: %d\n", status,
	(prev_priv[0]&PRV$M_SYSNAM) ? 1 : 0 );
    if ( is_sysgrp() ) printf 
	( "Warning, system group id gives implicit SYSPRV\n" );
    /*
     * Validate command line arguments, defaulting if needed.
     */
    conf_file[0] = '\0';
    log_file_name = "sys$output";
    rule_file_name = "";
    if ( argc > 1 ) log_file_name = argv[1];
    /*
     * Init log file and threaded file access module and read rules file.
     */
    http_log_level = DEFAULT_LOG_LEVEL;
    envval = getenv("HTDS_LOG_LEVEL");
    if ( envval ) {
	if ( (*envval == 'C') || (*envval == 'c') ) http_log_level = -1;
	else http_log_level = atoi ( envval );
    }

    status = tlog_init ( log_file_name );
    printf("Log file '%s' init status: %d\n\n", log_file_name, status );
    tlog_putlog ( 0,
	"DECThreads network object, version 1.1, 1-DEC-94, compiled !AZ !AZ!/", 
	__DATE__, __TIME__ );
    tf_initialize("");
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
    if ( ((status&1) == 0) || (acmode > 1) ) {
	server_mode = 0;		/* not a server */
	strcpy ( taskname, "sys$net" );
    } else {
	taskname[length] = '\0';
	server_mode = 1;
        if ( (prev_priv[0]&PRV$M_SYSNAM) == 0 ) printf ( "%s%s\n", "Warning,",
	  " insufficient privilege to declare DECnet object (SYSNAM required)");
    }
    /*
     * Setup access list.
     */
    status = ts_set_access ( "WWW_DIRSERV_ACCESS" );
    /*
     * Read miscellaneous parameters.
     */
    min_available = 0;
    envval = getenv("HTDS_MIN_AVAILABLE");
    if ( envval ) min_available = atoi ( envval );
    envval = getenv("HTDS_REENTRANT_C_RTL");
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
        client_limit = HTDS_DEFAULT_CLIENT_LIMIT;
        envval = getenv ( "HTDS_CLIENT_LIMIT" );
        if ( envval ) client_limit = atoi ( envval );
    } else client_limit = 1;
    cache_size = 0;

    tlog_putlog ( 0,
	"!/Port: !SL, Client limit: !SL, logger level: !SL!/", http_port,
		client_limit, http_log_level );
    if ( http_log_level > 0 ) status = tlog_initlog ( http_log_level,
	log_file_name );
    /*
     * Start listening on DECnet port.  Temporarily re-enable privs.
     */
    ts_set_logging ( tlog_putlog );
    pthread_attr_create ( &client_attr );
    pthread_attr_setinheritsched ( &client_attr, PTHREAD_DEFAULT_SCHED );
    pthread_attr_setstacksize ( &client_attr, 162000 );
    status = sys$setprv ( 1, priv_mask, 0, 0 );

    status = ts_declare_decnet_object 
	( taskname, client_limit, &client_attr, &http_thread, dir_request );
    (void) sys$setprv ( 0, priv_mask, 0, 0 );

    tlog_putlog(0,"Status of declare object (!AZ) = !SL!/", taskname, status);
    /*
     * Wait for DECnet communication thread to rundown.
     */
    if ( (status&1) == 1 ) {
	status = pthread_join ( http_thread, (void *) &exit_status );
    }
    else exit_status = status;
    if ( server_mode ) tlog_putlog 
		( 0, "!/listener thread exit status: !SL!/", exit_status );
    if ( status == 0 ) return exit_status;
    else return status;
}
/********************************************************************/
/* Utility routine to buffer output, accumulating small text fragments
 * into larger chunks for efficient I/O
 */

static int put_text ( struct out_stream *stream, char *ctlstr, ... )
{
    int used, status, i, count;
    char *buf, *text;
    va_list arg;
    used = stream->used;
    buf = &stream->buf[used];
    va_start ( arg, ctlstr );
    for (i = 0; ctlstr[i]; i++ ) {
	if ( (ctlstr[i] == '%') && 
	    ((ctlstr[i+1] == 's') || (ctlstr[i+1] == 't')) ) {
	    /* Replace %s in ctlstr with next argument */
	    i++;
	    if ( ctlstr[i] == 's' ) 
		for (text = va_arg(arg,char *); *text; used++) {
		    if ( used >= sizeof(stream->buf) ) {
	        	status = ts_decnet_write ( stream->ctx, stream->buf, used );
	        	if ( (status&1) == 0 ) return status;
	        	used = 0;
	        	buf = stream->buf;
		    }
	            *buf++ = *text++;
		}
	    else for (text = va_arg(arg,char *); *text; used++) {
		    if ( *text == '\n' ) {
		        if ( used >= sizeof(stream->buf) ) {
	        	    status = ts_decnet_write ( stream->ctx, 
					stream->buf, used );
	        	    if ( (status&1) == 0 ) return status;
	        	    used = 0;
	        	    buf = stream->buf;
		        }
			*buf++ = '\r';
			used++;
		    }
		    if ( used >= sizeof(stream->buf) ) {
	        	status = ts_decnet_write ( stream->ctx, stream->buf, used );
	        	if ( (status&1) == 0 ) return status;
	        	used = 0;
	        	buf = stream->buf;
		    }
	            *buf++ = *text++;
		}
	} else {
	    /* Add ctlstr character to output stream. */
	    if ( used >= sizeof(stream->buf) ) {
	        status = ts_decnet_write ( stream->ctx, stream->buf, used );
	        if ( (status&1) == 0 ) return status;
	        used = 0;
	        buf = stream->buf;
	    }
	    *buf++ = ctlstr[i]; used++;
	}
    }
    stream->used = used;
    return 1;
}
/*************************************************************************/
/* Main routine for handling htds server connection.  This function is called 
 * as the thread init routine for DECnet server threads.
 */
int dir_request ( void *ctx, 		/* TCP connection context */
	struct ncbdef *ncb,
	int ndx,			/* Thread index */
	int available )			/* # of contexts left on  free list */
{
    int length, status, tlog_flush(), ts_decnet_info(), i, tf_len, j;
    int header_only;
    string prologue[4];
    string request[128];
    char *full_name, *tail, prolog_buf[1104];

    char errmsg[256], dir_file[256], dirbuf[4096];
    char taskname[20], cre_date[32], remote_node[256], remote_user[64];
    char *bp, *opcode, *method, *url, *protocol, *module, log_prefix[32];
    struct out_stream outbound, *out;
    void *dirf;
    dir_opt opt;
    /*
     * Make prefix string for log entries so we can follow interleaved
     * entries in log file.  Log the connection with time.
     */
    ts_decnet_info ( taskname, remote_node, remote_user );
    tlog_putlog ( 3, "task: '!AZ', node: '!AZ', user: '!AZ'!/",
	taskname, remote_node, remote_user );
    tu_strcpy ( log_prefix, "DNT-" );
    tu_strcpy ( &log_prefix[4], taskname );
    i = tu_strlen(taskname) + 4; log_prefix[i++] = '/'; 
    tu_strint ( ndx, &log_prefix[i] );
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
     * Fetch binpath and treat as config file name.
     */
    if ( conf_file[0] == '\0' ) {
        ts_decnet_write ( ctx, "<DNETBINDIR>", 12 );
        status = ts_decnet_read ( ctx, conf_file, sizeof(conf_file)-1, &length );
        if ( (status&1) == 0 ) return status;
        conf_file[length] = '\0';
	status = htds_read_rules ( conf_file );
    }
    htds_get_rules ( (void *) 0, &opt );	/* Load global values */
    /*
     * Fetch original URL so we can put it in page title.
     */
    ts_decnet_write ( ctx, "<DNETRQURL>", 11 );
    status = ts_decnet_read ( ctx, dirbuf, sizeof(dirbuf)-1, &length );
    if ( (status&1) == 0 ) return status;
    dirbuf[length] = '\0';
    /*
     * Make pointers to portion of buffer with translated path and ending
     * location so we can easily append.
     */
    full_name = prologue[3].s;
    tail = &full_name[prologue[3].l];
    /*
     * Initialize structure for buffering output fragments into larger
     * DECnet message (much more efficient) and put connection in CGI mode.
     */
    outbound.ctx = ctx;
    outbound.used = 0;
    out = &outbound;
    ts_decnet_write ( ctx, "<DNETCGI>", 9 );	/* separate message */
    /*
     * Check method, must be GET or HEAD.  Set header_only flag if HEAD.
     */
    header_only = 0;
    if ( tu_strncmp ( prologue[1].s, "GET", 4 ) != 0 ) {
	if ( tu_strncmp ( prologue[1].s, "HEAD", 5 ) != 0 ) {
	    put_text ( out, "Status: 404 unsupported method\r\n%s",
	       "Content-type: text/plain\r\n\r\nError, unsupported method\r\n");
	    if ( outbound.used > 0 ) ts_decnet_write ( ctx, out->buf, out->used );
	    ts_decnet_write ( ctx, "</DNETCGI>", 10 );
	    status = ts_decnet_close ( ctx );
	    return 0;
	}
	header_only = 1;
    }
    /*
     * Search list of welcome files and redirect.
     */
    for ( i = 0; http_index_filename[i][0]; i++ ) {
	void *wf;
	tu_strcpy ( tail, http_index_filename[i] );
	if ( http_log_level > 3 ) 
		tlog_putlog(4,"Searching for '!AZ'!/", full_name );
	wf = tf_open ( full_name, "r", errmsg );
	if ( wf ) {
	    /* Found file, redirect using original path client requested */
	    tf_close ( wf );
	    put_text ( out, "Location: %s%s\r\n\r\n", dirbuf, tail );
	    if ( outbound.used > 0 ) ts_decnet_write ( ctx, out->buf, out->used );
	    ts_decnet_write ( ctx, "</DNETCGI>", 10 );
	    status = ts_decnet_close ( ctx );
	    return 1;
	}
    }
    /*
     * No welcome file, check restrictions on browsing:
     *    0 - no restrictions.
     *   -1 - Never browse.
     *    1 - Allow browse if control-file present (selective)
     *    2 - Allow browse if control-file present and read control file
     *        as additional config file rules (selective override).
     */
    if ( opt.access ) {
	void *cf;
	/* Look for control file. (selective) add*/
	tu_strnzcpy ( tail, opt.control_file, 80 );
	
	cf = tf_open ( full_name, "r", errmsg );
	if ( !cf ) opt.access = -2;
	else {
	    if ( opt.access > 1 ) htds_get_rules ( cf, &opt );
	    else tf_close ( cf );
	}

	if ( opt.access < 0 ) {
	    /*
	     * Browsing not allowed on this directory.
	     */
	    put_text ( out, "status: 403 Not browsable\r\n" );
	    put_text ( out, "content-type: text/plain\r\n\r\n" );
	    put_text (out, opt.access == -2 ?
		 "Directory not browsable (no %s file present).\r\n" :
		 "Directory browse disabled.\r\n", tail );
	    if ( out->used > 0 )
		status = ts_decnet_write ( ctx, out->buf, out->used );
	    ts_decnet_write ( ctx, "</DNETCGI>", 10 );
	    status = ts_decnet_close ( ctx );
	    return 1;
	}
    }
    /*
     * Open ident passed to us in prologue in special 'd' mode that
     * performs directory scan.  (reads return multiple filenames
     * separated by nulls)
     */
    *tail = '\0';
    dirf = tf_open ( full_name, "d", errmsg );
    if ( !dirf ) return 0;
    put_text ( out, 
	"status: 200 Directory listing follows\r\ncontent-type: text/html\r\n\r\n" );
    if ( out->used > 0 ) {
	status = ts_decnet_write ( ctx, out->buf, out->used );
	outbound.used = 0;
    }
    if ( header_only ) {
	/* don't return any data */
	ts_decnet_write ( ctx, "</DNETCGI>", 10 );
	tf_close ( dirf );
	status = ts_decnet_close ( ctx );
	if ( (status&1) == 0 )
        	tlog_putlog ( 0, "!AZ, status of close !SL!/", log_prefix, status );
        tlog_flush();
	return status;
    }
    /*
     * Generate header section of HTML output.
     */
    put_text ( out, "<HTML><HEAD><TITLE>Directory %s</TITLE></HEAD>\r\n",
		dirbuf);
    put_text ( out, "<BODY><DL>" );
    /*
     * Body of document is list.  Check for readme with TOP option.
     */
    if ( opt.readme == 1 ) {
	void *rf;
	tu_strcpy ( tail, opt.readme_file );
	rf = tf_open ( full_name, "r", errmsg );
	if ( rf ) {
	    put_text ( out, "<DT>Description</DT>\r\n<DD><PRE>" );
	    while ( 0 < (length=tf_read(rf,dirbuf,sizeof(dirbuf)-1) ) ) {
		dirbuf[length] = '\0';
		status = put_text ( out, "%t", dirbuf );
		if ( (status&1) == 0 ) return status;
	    }
	    tf_close ( rf );
	    put_text ( out, "</PRE></DD>\r\n" );
	}
    }
    /*
     * Read directory and convert to HTML.
     */
    list_files ( out, dirf, &opt, dirbuf, sizeof(dirbuf), full_name, tail );
    put_text(out, "</DL><HR></BODY></HTML>\r\n");
    /*
     * Body of document is list.  Check for readme with BOTTOM option.
     */
    if ( opt.readme == 2 ) {
	void *rf;
	tu_strcpy ( tail, opt.readme_file );
	rf = tf_open ( full_name, "r", errmsg );
	if ( rf ) {
	    put_text ( out, "<DT>Description</DT>\r\n<DD><PRE>" );
	    while ( 0 < (length=tf_read(rf,dirbuf,sizeof(dirbuf)-1) ) ) {
		dirbuf[length] = '\0';
		status = put_text ( out, "%t", dirbuf );
		if ( (status&1) == 0 ) return status;
	    }
	    tf_close ( rf );
	    put_text ( out, "</PRE></DD>\r\n" );
	}
    }
    /*
     * Flush remaining buffer and mark end of output.
     */
    if ( outbound.used > 0 ) ts_decnet_write ( ctx, out->buf, out->used );
    ts_decnet_write ( ctx, "</DNETCGI>", 10 );
    tf_close ( dirf );
    status = ts_decnet_close ( ctx );
    if ( (status&1) == 0 )
        tlog_putlog ( 0, "!AZ, status of close !SL!/", log_prefix, status );
    tlog_flush();
    return status;
}
/*************************************************************************/
static int list_files ( struct out_stream *out, void *dirf, dir_opt *opt,
	char dirbuf[4096], int bufsize, char *full_name, char *tail ) 
{
    int need_stat, i, j, k, length;
    stat_t statblk;
    /*
     * Make summary flag to indicate whether a stat() call is needed,
     * only incur the disk I/O if needed.
     */
    need_stat = opt->showsize || opt->showdate || opt->showowner || 
		opt->showgroup || opt->showprot;
    /*
     * We assume we are inside DL list, start new topic (files)
     */
    put_text ( out, "<DT>Files</DT>\r\n<DD><PRE>");
    /*
     * Scan directory.
     */
    while ( 0 < (length = tf_read(dirf, dirbuf, bufsize)) ) {
	/*
	 * tf_read returns multiple names in each buffer, process each.
	 */
	for ( i = j = 0; i < length; i++ ) if ( dirbuf[i] == '\0' ) {
	    void *fp;
	    /*
	     * dirbuf[j] is start of filename, skip immediately if
	     * hidden.
	     */
	    if ( (dirbuf[j] == '.') && !opt->showhidden ) {
		j = i + 1;
		continue;
	    }
	    /*
	     * Construct complete filename
	     */
	    tu_strcpy ( tail, &dirbuf[j] );
	    if ( ((i-j) > 4) && (tu_strncmp(&tail[i-j-4],".DIR", 5) == 0) ) {
		/*
		 *  We are a directory, convert '.DIR' to '/' and make
		 * hypertext refernece.
		 */
		tail[i-j-4] = '\0';
	        put_text(out, "<A HREF=\"%s/\">%s/</A>", tail, tail );
	    } else {
	        /*
	         * Make Anchor with relative hypertext reference.
	         */
		int st_sts, st_err, size;
		unsigned int uic, cdate, mdate;

	        put_text(out, "<A HREF=\"%s\">%s</A>", tail, tail );
		/*
		 * Only get stat info if we have to.
		 */
		if ( need_stat ) {
		    /*
		     * Extract header information from file.
		     */
		    CONDITIONAL_YIELD
		    LOCK_C_RTL
		    st_sts = stat ( full_name, &statblk );
		    if ( st_sts < 0 ) st_err = vaxc$errno;
		    UNLOCK_C_RTL

		    if ( st_sts == 0 ) {
			uic = statblk.st_uid;
		        size = statblk.st_size;
		        cdate = statblk.st_ctime;
		        mdate = statblk.st_mtime;
		    } else uic = size = cdate = mdate = 0;
		} else st_sts = 0;
		/*
		 * If status OK, add optional data if any.
		 */
	        if ( st_sts == 0 ) {
	            char number[20], scratch[32];
		    int k;
		    /*
		     * Format size, either as full byte or kilobytes.
		     */
		    if ( opt->showsize ) {
			if ( opt->showbytes ) {
			    put_text ( out, ", %s bytes",
				    tu_strint(size,number) );
			} else {
			    put_text ( out, ", %s Kb", 
				    tu_strint((size+999)/1000,number) );
			}
		    }
		    /*
		     * Fetch time, ctime() is non-reentrant, so get lock.
		     */
		    if ( opt->showdate ) {
			pthread_lock_global_np();
			tu_strncpy ( scratch, ctime ( (time_t *) &mdate ), 31 );
			pthread_unlock_global_np();
			/* Trim trailing '\n' added by ctime() */
			for (k = 0; scratch[k]; k++) if (scratch[k] == '\n') {
			    scratch[k] = '\0';
			    break;
			}
			put_text ( out, ", %s", scratch );
		    }
		}
	    }
 	    put_text(out,"\r\n");
	    j = i+1;		/* Advance to next name in buffer */
	}
    }
    /* Close up HTML list structures */
    put_text(out, "</PRE></DD>\r\n" );
    return 1;
}
