/*
 * This module handles management transactions requested by privileged
 * manage programs.  When the remote port and address of a connection is
 * the management port and address, the listener calls this routine rather
 * than create a normal client thread.
 *
 * Requests:				    return value
 *	SHUTDOWN[/grace-period]			1
 *	RESTART[/grace-period]			3
 *	INVCACHE				0
 *	NEWLOG					0
 *	NEWTRACE[/log-level]			0
 *      STATISTICS[/ZERO]			0
 *	HELP					0
 *
 * If the return value is non-zero, the port receiving the request is
 * shutdown and the thread exits with the return value as its status.
 *
 * The http_manage_request function is called directly by the port listener 
 * thread created by ts_declare_tcp_port() and therefore has the following
 * considerations:
 *
 *	> Context is not fully initialized, only safe actions are ts_tcp_read
 *	  and ts_tcp_write.
 *
 *	> New connections on the receiving port are suspended until this
 *	  routine returns.  Note that since more than one listener thread is
 *	  allowed, multiple active management requests are still possible.
 *
 *	> Stacksize is much smaller than for a normal client port, limiting
 *	  max request size
 *
 *	> Scheduler policy is FIFO, rather than DEFAULT, giving this routine
 *	  priority over client threads.
 *
 *  Author:	David Jones
 *  Date:	20-AUG-1994
 *  Revised:	26-AUG-1994	Support timeout argument on SHUTDOWN, RESTART.
 *  Revised:	27-AUG-1994	Support NEWLOG and NEWTRACE
 *  Revised:	21-APR-1995	Support STATISTICS
 *  Revised:	 2-SEP-1995	Include statistics counters in log file.
 */
#include <stdlib.h>
#include <stdio.h>
#include "tutil.h"
#include "tserver_tcp.h"
#include "counters.h"
int http_log_level;			/* global variable */
int tlog_putlog(), tlog_reopen();
static int convert_int ( char *string );
static int display_client ( void *ctx, struct client_counter *client );
static int display_hostcount ( void *ctx, struct host_counter *host );

static char *helpmsg = "200 Management commands:\r\n\
     HELP				Return this message\r\n\
     INVCACHE				Invalidate document cache\r\n\
     NEWLOG				Create new access log\r\n\
     NEWTRACE[/logger-level]		Create new trace file/set log level\r\n\
     RESTART[/timeout]			Restart server (default timeout=10)\r\n\
     SHUTDOWN[/timeout]			Shutdown (default timeout=10)\r\n\
     STATISTICS[/ZERO] 			Display counters\r\n\
\r\nThe command may be in either upper or lower case.";

int http_manage_request ( void *ctx,	/* I/O context */
	short port,			/* Local port number */
	int *shutdown_time )
{
    int status, i, length, valid;
    char *stsmsg, cmd[100], stsbuf[100];
    struct tu_streambuf stream;
    /*
     * Initialize stream and read command (delimited by newline).
     */
    tu_init_stream ( ctx, ts_tcp_read, &stream );

    status = tu_read_line ( &stream, cmd, sizeof(cmd)-1, &length );
    if ( (status&1) == 0 ) return 0;	/* ignore errors */
    /*
     * Upcase command string and process, set status to value to be returned.
     */
    status = 0;
    *shutdown_time = 10;		/* Default to 10 seconds */
    cmd[length] = '\0';
    tlog_putlog ( 1, "Management request to port !SW: '!AZ'!/", port, cmd );
    tu_strupcase ( cmd, cmd );
    if ( 0 == tu_strncmp(cmd,"SHUTDOWN",8) ) {
	/*
	 * Client is requesting port shutdown, check for optional
	 * parameter (number of seconds to give current sessions to complete).
	 */
	if ( cmd[8] == '/' ) *shutdown_time = convert_int ( &cmd[9] );
	/*
	 * Set return status to value that causes port to shutdown,
	 * waiting at most shutdown_time seconds.
	 */
	stsmsg = "201 Shutting down server port, active: ";
	status = 1;

    } else if ( 0 == tu_strncmp(cmd,"RESTART",7) ) {
	/*
	 * Client is requesting server restart, check for optional
	 * parameter (number of seconds to give current sessions to complete).
	 */
	if ( cmd[7] == '/' ) *shutdown_time = convert_int ( &cmd[8] );
	/*
	 * For restart, set status to value that causes port listener to
	 * rundown with status that causes server restart.
	 */
	stsmsg = "201 Restarting server, active clients: ";
        status = 3;

    } else if ( 0 == tu_strncmp(cmd,"HELP", 4) ) {
	/*
	 * Return statically declared help message.
	 */
	stsmsg = helpmsg;

    } else if ( 0 == tu_strncmp(cmd,"INVCACHE", 9) ) {
	/*
	 * Mark document cache invalid.
	 */
	int http_invalidate_doc_cache();
	http_invalidate_doc_cache();	/* clear cache */
	stsmsg = "200 Cache marked invalid\r\n";

    } else if ( 0 == tu_strncmp(cmd,"NEWLOG", 9) ) {
	/*
	 * Create a new access log file.
	 */
	if ( tlog_reopen ( -1 ) )
	    stsmsg = "200 New version of access log opened\r\n";
	else
	    stsmsg = "500 No access log or error on re-open\r\n";

    } else if ( 0 == tu_strncmp(cmd,"NEWTRACE", 8) ) {
	int level;
	/*
	 * Create a new trace file, setting optional logger level.
	 */
	if ( cmd[8] == '/' ) level = convert_int ( &cmd[9] );
	else level = http_log_level;

	if ( tlog_reopen ( level ) ) {
	    /*
	     * Format status line with old and new log levels.
	     */
	    stsmsg = stsbuf;
	    tu_strnzcpy ( stsmsg, 
		"200 New version of trace log opened at logger level: ", 60 );
	    i = tu_strlen ( stsmsg );
	    tu_strint ( level, &stsmsg[i] ); i = tu_strlen ( stsmsg );
	    tu_strnzcpy ( &stsmsg[i], ", old level: ", 15 );
	    i = tu_strlen ( stsmsg );
	    tu_strint ( http_log_level, &stsmsg[i] ); i = tu_strlen ( stsmsg );
	    stsmsg[i++] = '\r'; stsmsg[i++] = '\n';
	    /*
	     * The following is an unsafe (unsynchonized) update of 
	     * http_log_level.  We'll risk it because the worst that
	     * could happen is a thread in progress fails a test to 
	     * call tlog_putlog.
	     */
	    http_log_level = level;
	} else
	    stsmsg = "500 No trace log or error on re-open\r\n";

    } else if ( 0 == tu_strncmp(cmd,"STATISTICS", 10) ) {
	/*
	 * Get counters.
	 */
	if ( http_counters ) {
	    int i, count, total, ios;
	    tu_strcpy ( stsbuf,"200 counter display, zeroed at " );
	    http_lock_counters();
	    tu_strcpy ( &stsbuf[31], http_counters->zero_time );
	    ts_tcp_write ( ctx, stsbuf, tu_strlen(stsbuf) );
	    tlog_putlog(0,"MGR: counters snapshot at !%D, zeroed at !AZ!/", 0,
		       http_counters->zero_time );
	    ios = 1;
	    for ( total = i = 0; i < http_counters->active_size; i++ ) {
		count = http_counters->active_hist[i];
		if ( count <= 0 ) continue;
	        if ( total == 0 ) {
		    ts_tcp_write ( ctx, 
			"\r\n\r\nConcurrency histogram:", 26 );
		    tlog_putlog(0,
			"MGR: Concurrency histogram (active:count)!/");
		}
		total = total + count;
		tu_strcpy ( stsbuf, "\r\n         " );
		tu_strint ( i, &stsbuf[i < 10 ? 7 : i < 100 ? 6 : 
			i < 1000 ? 5 : 4] );
		stsbuf[8] = ':';
		tu_strint ( count, &stsbuf[10] );
		ios = ts_tcp_write ( ctx, stsbuf, tu_strlen(stsbuf) );
		if ( (ios&1) == 0 ) break;
		tlog_putlog(0,"MGR: !AZ!/", &stsbuf[2]);
	    }
	    tu_strcpy ( stsbuf, "\r\n\r\nTotal connects: " );
	    tu_strint ( total, &stsbuf[20] );
	    if ( (ios&1) ) ts_tcp_write ( ctx, stsbuf, tu_strlen(stsbuf) );
	    tlog_putlog(0,"MGR: !AZ!/", &stsbuf[4] );

	    if ( http_counters->active_sp > 0 ) {
		ts_tcp_write ( ctx,"\r\n\r\nPending requests:", 21 );
		for ( i = 0; i < http_counters->active_sp; i++ ) {
		    display_client ( ctx, &http_counters->active_stack[i] );
		}
	    }

	    if ( http_counters->host ) 
		display_hostcount ( ctx, http_counters->host );


	    if ( tu_strncmp ( &cmd[10], "/Z", 2 ) == 0 ) {
		http_zero_counters(1);
	        http_unlock_counters();
    		tlog_putlog ( 0, "MGR counters zeroed!/");
	    } else http_unlock_counters();

	    stsmsg = "\r\n";		/* handle final write */
	} else {
	    stsmsg = "500 Statistics counters not enabled\r\n";
	}

    } else {
	stsmsg = "400 Management request invalid, use HELP to list commands\r\n";
    }
    /*
     * Write response to client port and return.
     */
    ts_tcp_write ( ctx, stsmsg, tu_strlen ( stsmsg ) );
    return status;
}
/**************************************************************************/
/* Convert numeric string to integer. 
 */
static int convert_int ( char *str )
{
    int i, value;
    for ( value = i = 0; (str[i] >= '0') && (str[i] <= '9'); i++ ) {
	value = (value * 10) + (str[i] - (int)'0');
	if ( value > 99999999 ) break;
    }
    return value;
}
/****************************************************************************/
static int display_client ( void *ctx, struct client_counter *client )
{
    char line[100];
    int i, ndx, j,status,  port;

    /*
     * Format index number and remote host.
     */
    tu_strcpy ( line, "\r\n         " );
    ndx = client->ndx;
    tu_strint (ndx, &line[ndx < 10 ? 7 : ndx < 100 ? 6 : ndx < 1000 ? 5 : 4]);
    line[8] = ':';
    for ( i = 4, j = 10; i < 8; i++ ) {
	tu_strint ( client->address[i], &line[j] );
	while ( line[j] ) j++;
	line[j++] = '.';
    }
    line[j-1] = ':';
    port = client->address[2];
    tu_strint ( port*256 + client->address[3], &line[j] );
    while ( line[j] ) j++;
    if ( client->method ) {
	while ( line[j] ) j++;
	tu_strcpy ( &line[j], ", req: " ); j += 7;
    }
    status = ts_tcp_write ( ctx, line, j );
    if ( client->method ) {
	status = ts_tcp_write ( ctx, client->ident, tu_strlen(client->ident) );
    }
    return status;
}
/****************************************************************************/
/* Display list of host classes and counts */
static int display_hostcount ( void *ctx, struct host_counter *host )
{
    char line[100];
    int i, ndx, j,status,  port;
    ts_tcp_write ( ctx, "\r\n\r\nHost class counters:", 24 );
    tlog_putlog(0,"MGR: Host class counters:!/" );
    /*
     * Format each host class that has been defined.
     */
    for ( status = 1; host; host = host->next ) {
	tu_strcpy ( line, "\r\n   " );
	tu_strnzcpy ( &line[5], host->class_name, sizeof(line) - 18 );
	i = tu_strlen ( line );
        line[i++] = ':'; line[i++] = ' ';
	tu_strint ( host->count, &line[i] );
	while ( line[i] ) i++;
	status = ts_tcp_write ( ctx, line, i );
	if ( (status&1) == 0 ) break;
	tlog_putlog(0,"MGR: !AD!/", i-2, &line[2] );
    }
    return status;
}
