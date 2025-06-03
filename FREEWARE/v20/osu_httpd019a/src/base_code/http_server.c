/*
 * Thread-based TCP/IP HTTP server.
 *
 * Command-line format:
 *
 *	$ http_server err-file rules-file [http-port] [nocache-port2]
 *
 * args:
 *    log-file	    Name of log file to create, connections and query strings
 *		    are written to the log file.
 *
 *    rule-file	    Name of rules file.  The rules file maps aliases to
 *                  local files and restricts access.
 *
 *    http-port    Port number to listen for connects on.  Defaults to port
 *		    80.
 *
 *    nocache-port Port number to listen for connects on with caching disabled.
 *		   May be the same as http_port.  If different, a second
 *		   listener thread will be started.
 *
 * Environment variables:
 *    HTTP_CACHE_SIZE		If defined, sets maximum number of documents
 *				to be cached in memory.  Only documents that
 *				format to 4K bytes or less are cached.  Default
 *				value is 0 documents.
 *
 *    HTTP_CLIENT_LIMIT		If defined, limits maximum number of concurrent
 *				TCP connections the server is to support,
 *				default is 16.  At startup, current process
 *				quotas are checked and client limit will be
 *				reduced if needed to match the available
 *				resources.  See file client_limit.c for
 *				details.
 *
 *    HTTP_MIN_AVAILABLE	If defined, sets mininum number of clients free
 *				clients that must be available for the request
 *				to be processed.  If below this number, an
 *				error is returned.
 *
 *    HTTP_LOG_LEVEL		If defined, sets logging level (default=1):
 *				  common Use standard log file format.
 *				    0	Report errors, config file info.
 *				    1	Report connect/final stat info.
 *				    2	Report Identifier re-mapping
 *				    3	Report request/response byte counts.
 *				    5	Show decnet script transaction.
 *
 *    HTTP_DEFAULT_HOST		Internet host name to use for HTTP server
 *				redirects.  If not defined, re-directs on
 *				scripts will not be supported.  We use a
 *				an environment variable to keep TCP/IP
 *				operations to a minimum.
 *
 *    HTTP_REENTRANT_C_RTL	If defined, sets reentrancy for C runtime:
 *				    0	Library is not reentrant, global locks
 *					will be used to serialize RTL calls.
 *				    1	Library is reentrant, synchronization
 *					for library set to C$C_MULTITHREAD.
 *				If not defined, default value is 0.
 *
 *    HTTP_MANAGE_PORT		If defined, sets management port number for
 *				privileged management requests.
 *
 *    HTTP_MANAGE_HOST		If defined, set remote host the management
 *				requests come from.  Default is loopback
 *				address.  Specify this value as a signed
 *				numeric value.
 *
 *    HTTP_USER_ID		(Unix only).  Numeric user ID to setuid to
 *				after establishing listen port.
 *
 *    HTTP_GROUP_ID		(Unix only).  Numeric group ID to setgid to
 *				after establishing listen port.
 *
 * Logical names:
 *	WWW_SERVER_PORTS	Lists privileged ports server is permitted to 
 *				listen on.  Must be exec mode logical name.
 *
 *  Author:	David Jones
 *  Date:	12-FEB_1994
 *  Revised:	 9-MAR-1994	Added HTTP_HOST_NAME
 *  Revised:	27-MAY-1994	Added nocache-port support.
 *  Revised:	 7-JUN-1994	Make rule file errors fatal.
 *  Revised:	25-JUN-1994	Common log format.
 *  Revised:	23-JUL-1994	Do congestion checks.
 *  Revised:	29-JUL-1994	Set reentrancy of RTL dynamically at startup.
 *  Revised:	2-AUG-1994	Support of new DECNET_SEARCHLIST module.
 *  Revised:	20-AUG-1994	Add port manage and restart.
 *  Revised:	31-AUG-1994	Add http_server_version globals.
 *  Revised:	 1-SEP-1994	Fixed maxlen in tu_read_line (off by 1).
 *  Revised:	24-SEP-1994	Support ts_set_logging().
 *  Revised:	12-JAN-1995	Addded George Carrettes mods for localaddress
 *				(gjc@village.com).
 *  Revised:	24-FEB-1995	Support port rule in rule file.
 *  Revised:	21-APR-1995	Support statistics counters.
 *  Revised:     1-MAY-1995	Fixed tu_read_line() call, off by 1.
 *  Revised:	 4-MAY-1995	Add WWW_SERVER_PORTS logical.
 *  Revised:	23-MAY-1995	Fixes for OSF/1 version 3.
 *  Revised:	13-JUL-1995	Fix another bug handling buffer overflow
 *				on input loop.
 *  Revised:	15-JUL-1995	Redo declare_port calls to handle pooled
 *				client limits.
 */
#include "pthread_np.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef VMS
#include <unixlib.h>
#include <prvdef.h>
#include <lnmdef.h>
#include <descrip.h>

int http_max_server_clients();
#else  /* OSF */
#define http_max_server_clients(a,b) a
#define $DESCRIPTOR(a,b) static char *a = b
#if !defined(__DECC) && !defined(VAXC)
#include <sys/types.h>
uid_t http_saved_uid; gid_t http_saved_gid;
#endif
#endif
#include "tutil.h"		/* threaded utility routines */
#include "file_access.h"
#include "decnet_searchlist.h"
#include "tserver_tcp.h"	/* TCP/IP port listener */
#include "counters.h"		/* statistics counters */

int tlog_putlog ( int level, char * ctlstr, ... );
int init_rules();
int http_start_authenticator();
int http_client ( void *ctx, 
	short port, unsigned char *rem_address, int ndx, int available );
int http_manage_request ( void *ctx, short port, int *shutdown_time );
int http_init_doc_cache(), tlog_init();
int http_parse_elements(), http_read_rules ();
int http_set_cache_inhibit(), tlog_rundown();

#define RESPONSE_LINE_LIMIT 100
#define HTTP_DEFAULT_PORT 80
#define HTTP_DEFAULT_CLIENT_LIMIT 16
#define DEFAULT_CACHE_SIZE 0
#define DEFAULT_LOG_LEVEL 1
typedef struct { int l; char *s; } string;
/*
 * Global (program-wide) variables.
 */
int http_log_level;			/* Logging detail control */
char * http_default_host;		/* Host name for re-directs */
char *http_authenticator_image;		/* Program for authent. sub-process */
int http_dns_enable;			/* If true, lookup IP addresses */
int http_ports[2];			/* Rule-file port/nocache port */
char http_server_version[] = "1.9a";
char http_server_date[] = "8-SEP-1995";
static int min_available;
/**************************************************************************/
/* Main program. */
int main ( int argc, char **argv )
{
    int status, port_num, client_limit, exit_status, cache_size;
    char *envval, *log_file_name, *rule_file_name;
    pthread_t http_thread, http_thread2;
    pthread_attr_t client_attr;
#ifdef VMS
    long priv_mask[2], prev_priv[2], sys$setprv(), is_sysgrp();
    /*
     * Disable sysprv privilege.
     */
    priv_mask[0] = PRV$M_SYSPRV; priv_mask[1] = 0;
    status = sys$setprv ( 0, priv_mask, 0, prev_priv );
    printf("disable SYSPRV status: %d, prev set: %d\n", status,
	(prev_priv[0]&PRV$M_SYSPRV) ? 1 : 0 );
    if ( is_sysgrp() ) printf 
	( "Warning, system group id gives implicit SYSPRV\n" );
#else
#define sys$setprv(a,b,c,d) 1
#if !defined(__DECC) && !defined(VAXC) && !defined(VMS)
#include <pwd.h>
#include <grp.h>
    char *alt_user, *alt_group;
    /*
     * Determine uid/gid we are to run under and make that our effective ID,
     * leave real ID at root so we can enable it again.
     */
    http_saved_gid = getegid();		/* initial defaults */
    http_saved_uid = geteuid();
    if ( alt_user = getenv("HTTP_USER_ID") ) {
	/*
	 * Accept either #nnn or username form.
	 */
	if ( *alt_user == '#' ) http_saved_uid = atoi ( &alt_user[1] );
	else {
	    struct passwd *ent;
	    ent = getpwnam ( alt_user );
	    if ( ent ) {
		http_saved_uid = ent->pw_uid;
		http_saved_gid = ent->pw_gid;
	    }
	    else fprintf(stderr,"Error looking up UID for %s\n", alt_user );
	}
    }
    if ( alt_group = getenv("HTTP_GROUP_ID") ) {
	if ( *alt_group == '#' ) http_saved_gid = atoi ( &alt_group[1] );
	else {
	    struct group *ent;
	    ent = getgrnam ( alt_group );
	    if ( ent ) {
		http_saved_gid = ent->gr_gid;
	    }
	    else fprintf(stderr,"Error looking up group %s\n", alt_group );
	}
    }
    printf("Starting httpd with uid = %d and gid = %d\n",
	http_saved_uid, http_saved_gid );

    if ( 0 != setregid ( getgid (), http_saved_gid ) ) {
	printf("Unable to set gid\n");
	exit ( 1 );
    }
    if ( 0 != setreuid ( getuid (), http_saved_uid ) ) {
	printf("Unable to set uid\n");
	exit ( 1 );
    }
    /*
     * Make ourselves a detached process.
     */
    status = fork();
    if ( status < 0 ) printf( "Error detaching process\n");
    if ( status != 0 ) exit ( status > 0 ? 0 : 1 );
#endif
#endif
    /*
     * Validate command line arguments, defaulting if needed.
     */
    log_file_name = "sys$output";
    rule_file_name = "";
    if ( argc > 1 ) log_file_name = argv[1];
    if ( argc > 2 ) rule_file_name = argv[2];
    http_ports[0] = (argc > 3) ? atoi(argv[3]) : HTTP_DEFAULT_PORT;
    http_ports[1] = ( argc > 4 ) ? atoi(argv[4]) : 0;
    /*
     * Init log file and threaded file access module and read rules file.
     */
    http_counters = (struct counter_block *) 0;
    http_log_level = DEFAULT_LOG_LEVEL;
    http_default_host = NULL;
    envval = getenv("HTTP_LOG_LEVEL");
    if ( envval ) {
	if ( (*envval == 'C') || (*envval == 'c') ) http_log_level = -1;
	else http_log_level = atoi ( envval );
    }
    status = tlog_init ( log_file_name );
    printf("Log file '%s' init status: %d\n\n", log_file_name, status );
    tlog_putlog ( 0,
	"HTTP DECThreads server, version !AZ, !AZ, compiled !AZ !AZ!/", 
	http_server_version, http_server_date, __DATE__, __TIME__ );
    status = dnetx_initialize ( "WWWEXEC" );		/* Init node search lists */
    if ( (status&1) == 0 ) exit ( status );
    status = http_read_rules ( rule_file_name );	/* Load configuration*/
    if ( (status&1) == 0 ) exit ( status );
    tf_initialize("");
    min_available = 0;
    envval = getenv("HTTP_MIN_AVAILABLE");
    if ( envval ) min_available = atoi ( envval );
    envval = getenv("HTTP_REENTRANT_C_RTL");
    http_reentrant_c_rtl = envval ? atoi ( envval ) : 0;
#ifdef __DECC
    if ( http_reentrant_c_rtl ) {
	decc$set_reentrancy ( C$C_MULTITHREAD );
        tlog_putlog ( 0, "Set DECC library for multi-thread use!/" );
     }
#endif
    /*
     * Check for adequate privileges to on requested port.
     */
#ifdef VMS
    if ( (http_ports[0] < 1024) && ((prev_priv[0]&PRV$M_SYSPRV)==0) ) {
	printf("%s%s\n", "Warning, insufficient privilege to listen on ",
		"requested TCP port (SYSPRV required)" );
    }
    if ( (http_ports[0] < 1024) || (http_ports[1] < 1024) ) {
	int i, acmode, length, max_index, port_verified[2], n, SYS$TRNLNM();
	struct { short length, code; char *buffer; int *retlen; } item[4];
	char port_str[64];
	$DESCRIPTOR(table_name,"LNM$FILE_DEV");
	$DESCRIPTOR(port_logical,"WWW_SERVER_PORTS");
	item[2].code = LNM$_STRING; item[2].length = sizeof(port_str)-1;
	item[2].buffer = port_str; item[2].retlen = &length; length = 0;
	item[1].code = LNM$_ACMODE; item[1].length = sizeof(acmode);
	item[1].buffer = (char *) &acmode; item[1].retlen = (int *) 0;
	item[0].code = LNM$_MAX_INDEX; item[0].length = sizeof(max_index);
	item[0].buffer = (char *) &max_index; item[0].retlen = (int *) 0;
	item[3].code = item[3].length = 0;
	acmode = max_index = 0;
	port_verified[0] = (http_ports[0] < 1024) ? 0 : 1;
	port_verified[1] = (http_ports[1]>0)&&(http_ports[1]<1024) ? 0 : 1;
	for ( i = 0; i <= max_index; i++ ) {
	    status = SYS$TRNLNM ( 0, &table_name, &port_logical, 0, &item );
	    if ( ((status&1) == 0) || (acmode > 1) ) break;
	    item[0].code = LNM$_INDEX; item[0].buffer = (char *) &i;
	    port_str[length] = '\0';
	    n = atoi(port_str);
	    if ( n == http_ports[0] ) port_verified[0] = 1;
	    else if ( n == http_ports[1] ) port_verified[1] = 1;
	}
	if ( !port_verified[0] || !port_verified[1] ) {
	    printf("Cannot listen on privileged port unless verifed by %s logical\n",
		"WWW_SERVER_PORTS" );
	    exit ( 20 );
	}
      }
#endif
    /*
     * Set management stuff.
     */
    envval = getenv ( "HTTP_MANAGE_PORT" );
    if ( envval ) {
	int manage_port, manage_host;
	manage_port = atoi ( envval );
	envval = getenv ( "HTTP_MANAGE_HOST" );
	if ( envval ) manage_host = atoi ( envval );
	else manage_host = 0x0100007f;	/* default to loopback address */

	ts_set_manage_port ( manage_port, manage_host, http_manage_request );
	tlog_putlog ( 0, 
	    "Enabled management interface, requests must come from !UB.!UB.!UB.!UB.!UW!/",
	    manage_host, manage_host>>8, manage_host>>16, manage_host>>24,
	    manage_port );
    }
    /*
     * Start authenticator if defined in rules file.
     */
    if ( http_authenticator_image ) {
	status = http_start_authenticator ( http_authenticator_image );
    if ( http_log_level > 5 ) tlog_putlog ( 5,
	"authenticator image: !AZ, and status: !SL!/",
	http_authenticator_image, status );
    }
    /*
     * Initialize document cache.  Size set by environment variable.
     */
    cache_size = DEFAULT_CACHE_SIZE;
    envval = getenv ( "HTTP_CACHE_SIZE" );
    if ( envval ) {
	cache_size = atoi ( envval );
	tlog_putlog ( 0, "Setting cache size to !SL!/", cache_size );
    }
    status = http_init_doc_cache ( cache_size );
    http_set_cache_inhibit ( http_ports[1] );
    /*
     * Set host name global based on environment variable if rule file didn't
     * set it.
     */
    envval = getenv ( "HTTP_DEFAULT_HOST" );
    if ( (http_default_host == NULL) && envval ) {
	http_default_host = malloc ( strlen ( envval ) + 1 );
	strcpy ( http_default_host, envval );
    }
    /*
     * Compute client limit.
     */
    client_limit = HTTP_DEFAULT_CLIENT_LIMIT;
    envval = getenv ( "HTTP_CLIENT_LIMIT" );
    if ( envval ) client_limit = atoi ( envval );

    client_limit = http_max_server_clients ( client_limit, cache_size );
    tlog_putlog ( 0,
	"!/Port: !SL, Client limit: !SL, logger level: !SL!/", http_ports[0],
		client_limit, http_log_level );
    /*
     * Start listening on TCP ports.  Temporarily re-enable privs.
     */
    ts_set_logging ( tlog_putlog );
    pthread_attr_create ( &client_attr );
    pthread_attr_setinheritsched ( &client_attr, PTHREAD_DEFAULT_SCHED );
    pthread_attr_setstacksize ( &client_attr, 62000 );
    status = sys$setprv ( 1, priv_mask, 0, 0 );
#if !defined(__DECC) && !defined(VAXC) && !defined(VMS)
    if ( 0 != setreuid ( getuid (), getuid() ) ) {
	printf("Unable to set uid back to original\n");
	exit ( 1 );
    }
#endif
    status = ts_declare_tcp_port 
	(http_ports[0], client_limit, &client_attr, &http_thread, http_client);

    if ( http_ports[1] && (http_ports[1] != http_ports[0]) ) {
	/*
	 * Listen on no-cache port, using same client_attr will cause
	 * same client pool to be used.
	 */
        status = ts_declare_tcp_port 
	    ( http_ports[1], 0, &client_attr, &http_thread2, http_client );
    }
    (void) sys$setprv ( 0, priv_mask, 0, 0 );
#if !defined(__DECC) && !defined(VAXC) && !defined(VMS)
    setgid ( http_saved_gid );
    if ( 0 != setreuid ( getuid (), http_saved_uid ) ) {
	printf("Unable to  kill privs\n");
	exit ( 1 );
    }
#endif
    /*
     * Wait for TCP communication thread to rundown.
     */
    if ( status == 0 ) status = 
		pthread_join ( http_thread, (void *) &exit_status );
    else exit_status = status;
    tlog_putlog ( 0, "!/listener thread exit status: !SL!/", exit_status );
    if ( (status == 0) && (exit_status == 3) ) {
	/*
	 * Restart server 
	 */
#ifdef VMS
	char restart_cmd[1000];
	int length, CLI$GET_VALUE(), LIB$DO_COMMAND();
	$DESCRIPTOR ( entity, "$LINE" );
	$DESCRIPTOR ( cmd, "" );

	tlog_putlog(0,"Server restart requested: !%D!/", 0 );
	tlog_rundown();		/* close files */
	tu_strcpy ( restart_cmd, "MCR " );
	cmd.dsc$a_pointer = &restart_cmd[4];
	cmd.dsc$w_length = sizeof(restart_cmd) - 5;
	length = 0;
	status = CLI$GET_VALUE ( &entity, &cmd, &length );
	if ( (status&1) == 1 ) {
	    /*
	     * If $LINE doesn't start with "MCR prepend it.
	     */
	    cmd.dsc$w_length = length;
	    if ( 0!=tu_strncmp("MCR ", &restart_cmd[4], 4) ) {
	        cmd.dsc$a_pointer = restart_cmd;
	        cmd.dsc$w_length = length + 4;
	    }
	    /*
	     * Exit and re-run.
	     */
	    status = LIB$DO_COMMAND ( &cmd );
	}
#else
	tlog_rundown();
#if !defined(__DECC) && !defined(VAXC)
	execv ( argv[0], argv );
#endif
	printf("restart not supported yet.\n");
#endif
    }
    tlog_rundown();		/* close files */
    if ( status == 0 ) return exit_status;
    else return status;
}
/***********************************************************************/
/* Main routine for handling http server connection.  This function is called 
 * as the thread init routine for tcp-ip server threads.
 */
#define METHOD 0
#define URL 1
#define PROTOCOL 2

int http_client ( void *ctx, 		/* TCP connection context */
	short port, 			/* Port connection received on */
	unsigned char *rem_address, 	/* Address/port of client */
	int ndx,			/* Thread index */
	int available )			/* # of contexts left on  free list */
{
    int length, status, seg_len, tlog_flush();
    int i, lines, ts_tcp_write(), ts_tcp_read(), rem_port, http_execute();
    long start_time[2], end_time[2];
    string request[128];
    char reqbuf[4096];
    char *opcode, *method, *url, *protocol, log_prefix[32];
    char *ts_tcp_remote_host();
    struct tu_streambuf inbound;
    struct tu_textbuf response;
    /*
     * Make prefix string for log entries so we can follow interleaved
     * entries in log file.  Log the connection with time.
     */
    if ( http_counters ) {		/* Update server statistics */
	if ( http_counters->active_size ) 
		http_open_active_counter ( ndx, port, rem_address );
	if ( http_counters->host )
		http_increment_host_counters ( rem_address );
    }
    tu_strcpy ( log_prefix, "TCP-" );
    tu_strint( (int)((unsigned) port), &log_prefix[4] );
    i = 4 + tu_strlen(&log_prefix[4]); log_prefix[i++] = '/';
    tu_strint(ndx, &log_prefix[i] );

    start_time[0] = -1;
    if (http_log_level > 0 ) {
#ifdef VMS
	int SYS$GETTIM();
	SYS$GETTIM ( start_time );
#endif
        rem_port = rem_address[2];
	tlog_putlog ( 1, "!AZ connect from !UB.!UB.!UB.!UB.!UW, !%D (!SL)!/", 
	log_prefix, 
	rem_address[4], rem_address[5], rem_address[6], rem_address[7], 
	rem_port*256 + rem_address[3], start_time, available );
    }
    if ( http_dns_enable ) tlog_putlog ( 1,
	"!AZ host address translates to name !AZ!/", log_prefix,
		ts_tcp_remote_host() );
    /*
     * Read first line of request from network client and parse into
     * separate strings
     */
    tu_init_stream ( ctx, ts_tcp_read, &inbound );
    status = tu_read_line ( &inbound, reqbuf, sizeof(reqbuf)-21, &length );
    if ( (status&1) == 1 ) {
	reqbuf[length] = '\0';
	i = http_parse_elements ( 3, reqbuf, request );
    } else i = http_parse_elements ( 3, "{EOF}", request );
    /*
     * Validate command and fetch extended request info.
     */
    protocol = request[PROTOCOL].s;
    if ( request[PROTOCOL].l == 0 ) {
	/*
	 * Assume simple request.
         */
        if ( tu_strncmp ( request[METHOD].s, "GET", 4 ) ) {
	    request[3].l = -1;		/* indicate error */
	    request[3].s = "599 Invalid request";	/* detail */
	} else {
	    request[3].l = 0;
	    request[3].s = "";	/* Make null header line */
	}
	lines = 4;

    } else if ( (tu_strncmp ( protocol, "HTTP/", 5 ) == 0) ||
		(tu_strncmp ( protocol, "HTRQ/", 5 ) == 0) ) {
	/*
         * Protocols other than 1 are followed by header lines.
	 * Read lines until null line read.
         */
	for ( lines = seg_len = 3; seg_len > 0; lines++ ) {
	    /*
	     * make sure we have enough room to read more.
	     */
	    if ( (lines >= 126) || (length >= sizeof(reqbuf)-2) ) {
		request[lines].l = 0; 
		request[lines].s = &reqbuf[length];
		break;
	    }
	    length++;		/* Save terminating null */
	    status = tu_read_line ( &inbound, &reqbuf[length], 
			sizeof(reqbuf)-length-1, &seg_len );
    	    if ( (status&1) == 0 ) seg_len = 0;

	    request[lines].l = seg_len;
	    request[lines].s = &reqbuf[length];
	    length += seg_len;
	}
	if ( request[lines].l > 0 ) request[++lines].l = 0;
    } else {
	/*
	 * Unknown protocol in 3rd element.
	 */
	request[3].l = -1;
	request[3].s = "599 protocol error in request";
    }
    if ( available < min_available ) {
	request[3].l = -2;
	request[3].s = "502 Server overload, request aborted";
    }
    if ( http_counters ) if ( http_counters->active_size ) {
	/* record what the active thread is fetching */
	http_set_active_counter ( ndx, request[METHOD].s, request[URL].s );
    }
    if ( http_log_level > 0 ) 
	tlog_putlog ( 1,"!AZ http request: '!AZ' '!AZ', (!AZ)!/",
	    log_prefix, request[0].s, request[1].s, request[2].s );
    if ( http_log_level > 2 )
	tlog_putlog ( 1, "!AZ      request stats: strings=!SL, bytes=!SL!/",
	    log_prefix, lines, length );

    status = http_execute ( ctx, log_prefix, request, &inbound );
    if ( (status&1) == 1 ) (void) ts_tcp_close ( ctx );

    if ( start_time[0] != -1 ) {
#ifdef VMS
	long delta_time[2], divisor, offset, msec, LIB$EDIV(), LIB$SUBX();
	int SYS$GETTIM();
	SYS$GETTIM ( end_time );
	LIB$SUBX ( end_time,  start_time, delta_time );
	divisor = 10000;	/* millisecond */
	LIB$EDIV ( &divisor, delta_time, &msec, &offset );
#else
	long msec = 0;
#endif
	tlog_putlog ( 1,
	"!AZ execution completed with status: !SL at !%T (!SL)!/",
	log_prefix, status, end_time,  msec );
    }
    /*
     * Processing done, cleanup.
     */
    if ( http_counters ) if ( http_counters->active_size )
	http_close_active_counter ( ndx );
    tlog_flush();
    return status;
}
