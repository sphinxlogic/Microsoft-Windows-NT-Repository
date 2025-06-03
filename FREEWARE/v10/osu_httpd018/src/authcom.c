/*
 * This module handles level 2 access protection for the HTTP server.  The
 * level 2 protection checks are actually done by a sub-process that this
 * module creates and communicates with via a mailbox.
 *
 * http_start_authenicator ( char *image );
 * int http_authenicate ( access_info acc, char *ident, tu_test rsphdr )
 *
 * Revised: 1-JUL-1994		Hack username into log line.
 * Revised: 30-JUl-1994		Fixup problem with username in log line.
 * Revised: 26-AUG-1994		Missing unlock in rundown routine.
 * Revised: 25-SEP-1994		Add tserver_tcp.h header file.
 * Revised:  4-FEB-1995		Add spawn flags for increased security (from 
 *				Chuck Lane, lane@duphy4.physics.drexel.edu).
 * Revised:  8-FEB-1995		Log spawn errors.
 */
#include <stdio.h>
#include <descrip.h>
#include <ctype.h>
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
#include <clidef.h>
#ifndef CLI$M_TRUSTED
/* clidef.h is out of date, hard code new values */
#define CLI$M_TRUSTED 0		/* 0x0040 */
#define CLI$M_AUTHPRIV 0	/* 0x0080 */
#endif

#include <string.h>

#include "pthread_np.h"
#include "tutil.h"
#include "tserver_tcp.h"
#include "access.h"

#define REQUEST_MAILBOX "HTTP_AUTHCOM_RSP"
#define RESPONSE_MAILBOX "HTTP_AUTHCOM_REQ"
typedef struct { int l; char *s; } string;
int http_log_level;
int tlog_putlog ( int level, char *ctlstr, ... );
static int ef;
static int req_mbx, rsp_mbx;
static long auth_pid = 0;			/* authenticator's process ID */

#include "authcom.h"
static struct msg1 req_msg;		/* Request mailbox messages */

static struct msg2 rsp_msg;             /* response mailbox messages. */

static struct {
    unsigned short status, count;	/* status and bytes read/written */
    long pid;				/* PID of sending/receiving process */
} iosb;

static pthread_mutex_t auth_io;		/* Mutex for I/O condition */
static pthread_cond_t auth_avail;	/* Signals that authenticator free */
static pthread_cond_t auth_io_done;	/* Signals that mbx I/O completed */
static pthread_key_t auth_key;
static pthread_t auth_owner;		/* Thread I/D of currnent owner */
static int auth_busy;			/* If true, authenticator in use */
static int auth_waiting;		/* Flags if thread is blocked */

/******************************************************************************/
/*
 * The following two routines are used to enable a thread to perform I/O 
 * without blocking the execution of other threads.  The thread must do
 * the following:
 *	1. Acquire auth_io mutex.
 *	2  call SYS$QIO function specifying auth_synch_ast as the
 *	   AST argument and address of auth_io_done the as the AST parameter.
 *	3. call auth_synch_io to get status and release the mutex.
 */
static int auth_synch_ast ( pthread_cond_t *condition )
{
    int status;
    /*
     * Signal any waiting threads.
     */
    status = pthread_cond_signal_int_np ( condition );
    return status;
}

static int auth_synch_io ( int status, pthread_cond_t *condition )
{
    /*
     * Make sure operation is pending (success status).
     */
    if ( (status&1) == 1 ) {
	/*
	 * Loop until predicaate (ctx->iosb.status) is non-zero.
	 */
	do {
	    pthread_cond_wait ( condition, &auth_io );
	} while ( iosb.status == 0 );
	status = iosb.status;
    }
    /*
     * Condition satisfied, unlock mutex.
     */
    pthread_mutex_unlock ( &auth_io );
    return status;
}
/***************************************************************************/
/* Allocate access to the authenticator for this thread, waiting if another
 * thread using it.
 */
static void allocate_authenticator()
{
    /*
     * Serialize access to this routine through use of auth_avail condition.
     */
    pthread_mutex_lock ( &auth_io );
    if ( auth_busy ) pthread_setspecific ( auth_key, (void *) 2 );
    while ( auth_busy ) {
	auth_waiting++;
	pthread_cond_wait ( &auth_avail, &auth_io );
	auth_waiting--;
    }
    auth_busy = 1;
    pthread_setspecific ( auth_key, (void *) 1 );	/* mark that we own it */
    pthread_mutex_unlock ( &auth_io );
}
/***************************************************************************/
/* Deallocate the authenticator, signalling any other threads waiting.
 * This routine assumes that the calling thread owns the authenticator.
 */
void deallocate_authenticator()
{
    pthread_mutex_lock ( &auth_io );
    auth_busy = 0;
    pthread_setspecific ( auth_key, (void *) 0 );	/* no longer own authen. */
    if ( auth_waiting > 0 ) pthread_cond_signal ( &auth_avail );
    pthread_mutex_unlock ( &auth_io );
}
/***************************************************************************/
/* Cleanup any active contexts associated with a thread.  Active_flag is
 * the current auth_key context value:
 *     0 - Don't own authenticator, not waiting.
 *     1 - Currently 'own' authenticator.
 *     2 - Currently waiting for access to authenticator.
 */
static void auth_rundown ( void *active_flag_arg )
{
    int active_flag;
    active_flag = (int) active_flag_arg;	/* cast into int */
    if ( active_flag == 0 ) return;	/* not accessing authenticator */

    if ( auth_busy  && (active_flag == 1) ) {
	/*
	 * We are the active thread.  Kill any pending I/O.
	 */
	int SYS$CANCEL();
	SYS$CANCEL ( rsp_mbx );
	SYS$CANCEL ( req_mbx );
	pthread_mutex_lock ( &auth_io );
	if ( auth_waiting ) pthread_cond_signal ( &auth_avail );
	auth_busy = 0;
	pthread_mutex_unlock ( &auth_io );

    } else if ( active_flag == 2 ) {
	/*
	 * We were waiting to allocate it, reduce reference count.
	 */
	pthread_mutex_lock ( &auth_io );
	auth_waiting--;
	pthread_mutex_unlock ( &auth_io );
    }
}

/***************************************************************************/
static int send_request ( int length )
{
    int status, SYS$QIO ();
    /*
     * write message, retry until the authenticator is the reader.
     * If auth_pid is zero, loop will not execute and return NOREADER status.
     */
    status = SS$_NOREADER;
    for ( iosb.pid = 0; auth_pid != iosb.pid; ) {
	pthread_mutex_lock ( &auth_io );
	status = SYS$QIO ( ef, req_mbx, IO$_WRITEVBLK|IO$M_READERCHECK,
		&iosb, auth_synch_ast, &auth_io_done, 
		&req_msg, length, 0, 0, 0, 0 );
	status = auth_synch_io ( status, &auth_io_done );
	if ( (status&1) == 0 ) {
	    if ( status == SS$_NOREADER ) auth_pid = 0;
	    break;
	}
    }
    return status;
}
static int receive_response ( int *length )
{
    int status, SYS$QIO ();
    /*
     * Read message, ignore any that aren't from the authenticator.
     */
    status = SS$_NOWRITER;
    for ( iosb.pid = 0; auth_pid != iosb.pid; ) {
	pthread_mutex_lock ( &auth_io );
	status = SYS$QIO ( ef, rsp_mbx, IO$_READVBLK|IO$M_WRITERCHECK,
		&iosb, auth_synch_ast, &auth_io_done, 
		&rsp_msg, sizeof(rsp_msg), 0, 0, 0, 0 );
	status = auth_synch_io ( status, &auth_io_done );
	if ( (status&1) == 0 ) {
	    if ( status == SS$_NOWRITER ) auth_pid = 0;
	    break;
	}
    }
    *length = iosb.count;
    return status;
}
/***************************************************************************/
/*
 * Authenticator startp initializes the communcation layer.  This must
 * be done prior to creating any threads.
 */
int http_start_authenticator ( char *image )
{
    int status, spawn_flags, LIB$SPAWN();
    int LIB$GET_EF(), SYS$CREMBX(), SYS$QIO(), SYS$SYNCH(),SYS$CANCEL();
    char command[256];
    $DESCRIPTOR(req_mbx_name,REQUEST_MAILBOX);
    $DESCRIPTOR(rsp_mbx_name,RESPONSE_MAILBOX);
    $DESCRIPTOR(cmd, "");
    /*
     * First create mailboxes.  Make them directional so we can detect the
     * authenticator dying on us.
     */
    req_mbx = rsp_mbx = ef = 0;
    LIB$GET_EF ( &ef );
    status = SYS$CREMBX ( 0, &req_mbx, sizeof(req_msg), sizeof(req_msg),
		MBX_PROT, 0, &req_mbx_name, CMB$M_WRITEONLY );
    if ( (status&1) == 0 ) return status;
    status = SYS$CREMBX ( 0, &rsp_mbx, sizeof(rsp_msg), sizeof(req_msg),
		MBX_PROT, 0, &rsp_mbx_name, CMB$M_READONLY );
    if ( (status&1) == 0 ) return status;
    /*                        
     * Start asynch read on response mailbox.
     */
    status = SYS$QIO ( ef, rsp_mbx, IO$_READVBLK, &iosb, 0, 0,
		&rsp_msg, sizeof(rsp_msg), 0, 0, 0, 0 );
    if ( (status&1) == 0 ) return status;
    /*
     * Spawn image, passing mailbox names as command line arguments.  Save PID
     * of the sub-process to validate against incoming messages.
     */
    sprintf ( command, "mcr %s %s %s", image, 
		REQUEST_MAILBOX, RESPONSE_MAILBOX);
    cmd.dsc$w_length = strlen ( command );
    cmd.dsc$a_pointer = command;

    spawn_flags = CLI$M_NOWAIT | CLI$M_TRUSTED | CLI$M_AUTHPRIV;
    status = LIB$SPAWN ( &cmd, 0, 0, &spawn_flags, 0, &auth_pid, 0, 0,
		SYS$CANCEL, rsp_mbx );
    if ( (status&1) == 0 ) {
	tlog_putlog (0, "Error spawning authenticator, status: 0x!XL!/", status);
	return status;
    }
    tlog_putlog ( 0, "Created authenticator process, PID = !8XL!/", auth_pid );
    /*
     *  After initializing, the authenticator writes a status line to the 
     *  mailbox.  Wait on this message to arrive, which lets us know the
     *  authenticator started.  The spawn specified SYS$CANCEL as the
     *  completion AST so that if the image dies the read will fail with
     *  an abort status.
     */
    status = SYS$SYNCH ( ef, &iosb );
    if ( (status&1) == 1 ) status = iosb.status;
    if ( (status&1) == 0 ) {
	tlog_putlog ( 0, "Error, authenticator did not initialize!/" );
	auth_pid = 0;
	return status;
    }
    tlog_putlog ( 0, "!AF!/", rsp_msg.log_len, &rsp_msg.data[rsp_msg.sts_len] );
    /*
     * Create DECthread objects used to serialize access to the authenticator.
     */
    auth_busy = 0;
    auth_waiting = 0;
    status = pthread_mutex_init ( &auth_io, pthread_mutexattr_default );
    status = pthread_keycreate ( &auth_key, auth_rundown );
    status = pthread_cond_init ( &auth_avail, pthread_condattr_default );
    status = pthread_cond_init ( &auth_io_done, pthread_condattr_default );

    return 1;
}

/***************************************************************************/
static void append_authorize_header (string *request, char *buffer, 
	int *result_length, int maxlen )
{
    int i, j, req_len, length, line_matched;
    char *p, *t, c;
    /*
     * Scan header lines.
     */
    length = *result_length;
    line_matched = 0;
    t = "authorization";
    for ( i = 3; (req_len = request[i].l) > 0; i++ ) {
	p = request[i].s;
	if ( !isspace(*p) ) {
	    line_matched = 0;
	    for ( j = 0; (j < req_len) && (p[j] != ':'); j++ ) {
		c = _tolower(p[j]);
		if ( c != t[j] ) break;		/* no match */
	    }
	    if ( (j < req_len) && (p[j] == ':') ) line_matched = 1;
	}
	if ( !line_matched ) continue;
	if ( req_len + length + 2 > maxlen ) continue;	/* won't fit */
	/*
	 * Append line to buffer and append linefeed.
	 */
	tu_strcpy ( &buffer[length], p );
	length = tu_strlen ( buffer );
	buffer[length++] = '\n'; buffer[length] = '\0';
    }
    *result_length = length;
}

/******************************************************************************/
/* Main routine for checking authentication.
 */
int http_authenticate ( access_info acc,	/* object access */
	char *ident,				/* Ident to access */
	string *request,			/* Request header lines */
	tu_text rsphdr )			/* Error response */
{
    int status, i, http_add_response(), length, flag_skip;
    /*
     * Check that authenticator is running.
     */
    if ( auth_pid == 0 ) {
	/*
	 * No authenticator, put error response in response header and
	 * return fail status.
	 */
        status = http_add_response ( rsphdr, "500 Internal error", 1 );
	return 0;
    }
    /*
     * Serialize access to this routine through use of auth_avail condition.
     */
    allocate_authenticator();
    /*
     * Build message.  Trim leading '+' from protection file name.
     */
    req_msg.code = 1;
    ts_tcp_info ( &req_msg.local_port, &req_msg.remote_port,
	(unsigned int *) &req_msg.remote_addr );
    length = request[0].l;
    tu_strnzcpy ( req_msg.method, request[0].s, length > 7 ? 7 : length );
    flag_skip = (*acc->prot_file == '+') ? 1 : 0;
    req_msg.setup_len = tu_strlen ( &acc->prot_file[flag_skip] );
    if ( req_msg.setup_len > 255 ) req_msg.setup_len = 255;
    req_msg.ident_len = tu_strlen ( ident );
    if ( req_msg.ident_len > 256 ) req_msg.setup_len = 256;
    tu_strncpy ( req_msg.data, &acc->prot_file[flag_skip], req_msg.setup_len );
    tu_strncpy ( &req_msg.data[req_msg.setup_len], ident, req_msg.ident_len );
    length = req_msg.setup_len + req_msg.ident_len;
    /*
     * Scan request headers for authorization lines and append to request.
     */
    append_authorize_header ( request, req_msg.data, &length, 
		sizeof(req_msg.data) );
    /*
     * send message to authenticator.
     */
    length = length + REQ_HDR_SIZE;
    status = send_request(length);
    if ( (status&1) == 0 ) {
        status = http_add_response ( rsphdr, "500 Internal error", 1 );
	status = 0;
    } else {
        /*
         * read response.
         */
        status = receive_response ( &length );
        if ( (status&1) == 0 ) {
            status = http_add_response ( rsphdr, "500 Internal error", 1 );
	    status = 0;
        }
	else {
	    /*
	     * Set return status from message.  If access failed, place
	     * status line in response header.
	     */
	    status = rsp_msg.status;
	    if ( !status ) {
		/* Strip final newline from status line. */
		if ( rsp_msg.data[rsp_msg.sts_len-1] == '\n' )
		    rsp_msg.data[rsp_msg.sts_len-1] = '\0';
		if ( rsp_msg.data[rsp_msg.sts_len-2] == '\r' )
		    rsp_msg.data[rsp_msg.sts_len-2] = '\0';
		http_add_response ( rsphdr, rsp_msg.data, 1 );
	    }
	    /*
	     * Hack to get username from authenticator.  If log line
	     * begins with "[string]", then string is username to insert
	     * in acc structure.
	     */
	    if ( (rsp_msg.log_len > 2) && 
		 (rsp_msg.data[rsp_msg.sts_len] == '[') ) {
		char c;
		/*
		 * Copy porition within brackets to acc.
		 */
		rsp_msg.sts_len++;	/* skip '[' */
		rsp_msg.log_len--;
		for ( i = 0; rsp_msg.log_len > 0 &&
			i < sizeof(acc->rem_user)-1; i++ ) {
		    --rsp_msg.log_len;
		    c = rsp_msg.data[rsp_msg.sts_len++];
		    if ( (c == '\0') || (c == ']') ) break;
		    acc->user[i] = c;
		}
		acc->user[i] = '\0';	/* terminate string */
	    }
	    /*
	     * Add log line if present.
	     */
	    if ( (rsp_msg.log_len > 0) && (http_log_level > 1) ) 
		tlog_putlog ( 1, "!AF!/",
		 rsp_msg.log_len, &rsp_msg.data[rsp_msg.sts_len] );
	}
    }
    /*
     * Free access to the channel.
     */
    deallocate_authenticator();

    return status;
}
/******************************************************************************/
/* Routine for requesting an auxilly network service (code 2) from the
 * authenticator.
 */
int http_auth_netsrv ( char *subfunc,		/* Subfunc: REMHOST, REMUSER */
	int tag,
	char *arg, int arglen,
	char *response,
	int rsplen )
{
    int status, i, http_add_response(), length, flag_skip;
    /*
     * Check that authenticator is running.
     */
    if ( auth_pid == 0 ) {
	/*
	 * No authenticator, put error response in response header and
	 * return fail status.
	 */
	return 0;
    }
    /*
     * Serialize access to this routine through use of auth_avail condition.
     */
    allocate_authenticator();
    /*
     * Build message.  Trim leading '+' from protection file name.
     */
    req_msg.code = 2;		/* Network service */
    ts_tcp_info ( &req_msg.local_port, &req_msg.remote_port,
	(unsigned int *) &req_msg.remote_addr );
    tu_strnzcpy ( req_msg.method, subfunc, 7 );
    req_msg.setup_len = tag;
    req_msg.ident_len = arglen;
    if ( arglen > sizeof(req_msg.data) ) 
	req_msg.ident_len = sizeof(req_msg.data);
    tu_strncpy ( req_msg.data, arg, req_msg.ident_len );
    /*
     * send message to authenticator.
     */
    length = req_msg.ident_len + REQ_HDR_SIZE;
    status = send_request(length);
    if ( (status&1) == 1 ) {
        /*
         * read response.
         */
        status = receive_response ( &length );
        if ( (status&1) == 1 ) {
	    /*
	     * Set return status from message.  If access failed, place
	     * status line in response header.
	     */
	    status = rsp_msg.status;
	    if ( !status ) {
		/* Copy data to user arg */
		length = rsplen-1;
		if ( length > rsp_msg.sts_len ) length = rsp_msg.sts_len;
		tu_strnzcpy ( response, rsp_msg.data,length );
	    }
	    /*
	     * Add log line if present.
	     */
	    if ( (rsp_msg.log_len > 0) && (http_log_level > 1) ) 
		tlog_putlog ( 1, "!AF!/",
		 rsp_msg.log_len, &rsp_msg.data[rsp_msg.sts_len] );
	}
    }
    /*
     * Free access to the channel.
     */
    deallocate_authenticator();

    return status;
}
