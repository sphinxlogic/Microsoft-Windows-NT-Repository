/*
 * This module provides a wrapper script for executing CGI scripts via
 * fork()/exec() instead of the WWWEXEC DECnet object.  The script_execute()
 * code connects to the $cgifork service which creates an MST (Message-based
 * Service Thread) executing the cgifork_mst().
 *
 * The MST reads the parameters for the script request and builds a CGI 1.1 
 * environment array.  It then forks and has the child fork execute
 * requested script.  The output of the script is recieved over a pipe
 * and repackaged for the scriptserver protocol.
 *
 *   int cgifork_configure ( )
 *
 * Author: David Jones
 * Date:   19-MAY-1995
 * Revised: 23-MAY-1995		! cleanup error path
 * Revised: 31-MAY-1995		accomodate presentation scripts.
 * Revised:  4-JUN-1995		Set real UID to non-prived in child fork.
 * Revised: 12-JUN-1995		Work-around out-of-date iodef.h files.
 * Revised: 13-JUN-1995		Remove __DECC dependency.
 */
#include "pthread_np.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "tutil.h"
#include "message_service.h"		/* mst support routines */
#include "file_access.h"
#if defined(__DECC) || defined(VMS)
/*
 * do kludges for faking fork. (lets code be tested under VMS).
 */
#ifndef VMS
#define VMS
#endif
#define MAX_ENV_SIZE 120
#include <unixlib.h>
#include <processes.h>
#include <unixio.h>
#include <file.h>
#include <descrip>
#include <starlet>
#include <lnmdef>
#include <iodef>
#ifndef IO$M_WRITERCHECK
/* Out-of-date iodef.h, hard code the value for mailbox driver option */
#define IO$M_WRITERCHECK 0x0200
#endif
#ifndef CMB$M_READONLY
#define CMB$M_READONLY 1
#define CMB$M_WRITEONLY 2
#else
#include <cmbdef.h>
#endif
#include <ssdef>
#include <dvidef>
#define unlink(a) delete(a)

static $DESCRIPTOR(sysin, "CGI_CONTENT_FILE");
static $DESCRIPTOR(sysout, "SYS$OUTPUT");
static $DESCRIPTOR(log_table, "LNM$PROCESS");
static int setup_child_io(), cleanup_child_io(), read_from_child(),
	close_mailbox();
#else
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
uid_t http_saved_uid; gid_t http_saved_gid;
/*
 * The process descriptor table size.
 */
static int cgifork_fdtablesize;
#endif
int http_log_level, tlog_putlog();
int cgifork_mst
	( mst_link_t link, char *service, char *info, int ndx, int avail );
/********************************************************************/
/* Setup routine called to initialize module.
 */
int cgifork_configure ( )
{
#ifndef VMS
    /*
     * Get size of file descriptor table.
     */
    cgifork_fdtablesize = getdtablesize();
#endif
    return 1;
}

struct env_buffer {
    int count;			/* next envp index to use */
    int used;			/* Characters consumed */
    char *ptr[200];
    char buf[4096];
};
/********************************************************************/
static int error_abort ( mst_link_t link, char *sts_line, char *message )
{
    int status, written;
    status = mst_write ( link, "<DNETCGI>", 9, &written );
    if ( (status&1) == 0 ) return status;
    status = mst_write ( link, "status: ", 9, &written );
    if ( (status&1) == 0 ) return status;
    status = mst_write ( link, sts_line, tu_strlen(sts_line), &written );
    if ( (status&1) == 0 ) return status;
    status = mst_write ( link, "\r\ncontent-type: html\r\n", 22, &written );
    if ( (status&1) == 0 ) return status;
    status = mst_write ( link, message, tu_strlen(message), &written );
    if ( (status&1) == 0 ) return status;
    status = mst_write ( link, "\r\n", 1, &written );
    if ( (status&1) == 0 ) return status;
    status = mst_write ( link, "</DNETCGI>", 10, &written );
    return status;
}
/********************************************************************/
/*  Build array of character strings for execel call.
 */
static int add_var ( char *name, char *value, int vlen, struct env_buffer *env )
{
    int i, nlen;

    if ( env->count >= 199 ) return -1;
    nlen = tu_strlen(name);
    if ( (nlen+vlen+2+env->used) > sizeof(env->buf) ) return -1;

    env->ptr[env->count++] = &env->buf[env->used];
    tu_strcpy ( &env->buf[env->used], name ); env->used += nlen;
    env->buf[env->used++] = '=';
    tu_strnzcpy ( &env->buf[env->used], value, vlen ); env->used += vlen;
    env->buf[env->used++] = '\0';
    return 1;
}
/********************************************************************/
static int merge_var ( char *name, char *value, int vlen, 
	struct env_buffer *env )
{
    int i, j, nlen;
    /*
     * Scan for matching name.
     */
    if ( vlen+2+env->used > sizeof(env->buf) ) return -1;	/* won't fit */
    nlen = tu_strlen(name);
    for ( i = env->count-1; i >= 0; --i ) {
	if ( tu_strncmp ( name, env->ptr[i], nlen ) == 0 ) {
#ifdef VMS
	    if ( tu_strlen(env->ptr[i]) + vlen > MAX_ENV_SIZE ) continue;
#endif
	    if ( env->ptr[i][nlen] = '=' ) break;
        }
    }
    if ( i < 0 ) {
	/* Not found, append new name to end. */
	return add_var ( name, value, vlen, env );
    } else if ( i == env->count-1 ) {
	/*
	 * tack on to last variable.
	 */
	env->buf[env->used-1] = ',';
	env->buf[env->used++] = ' ';
	tu_strnzcpy ( &env->buf[env->used], value, vlen );
	env->used += (vlen+1);
	return 1;
    } else {
	/*
	 * Insert new data into list.
	 */
	char *base, *high;
	base = env->ptr[i+1] - 1;
	high = &env->buf[env->used];
	while ( high > base ) { high[vlen+2] = *high; --high; }
	for ( j = i+1; j < env->count; j++ ) env->ptr[j] += (vlen+2);
	*base++ = ',';
	*base++ = ' ';
	tu_strnzcpy ( base, value, vlen );
	env->used += (vlen+2);
	return 1;
    }
}
/********************************************************************/
/* Scan the request headers and convert into variables on the
 * environment list of for "HTTP_label".
 */
static void convert_headers ( mst_link_t link, struct env_buffer *env, 
	char *buffer, int *content_length )
{
    char label[256];
    int status, length, colon, label_len;

    *content_length = 0;
    status = mst_write ( link, "<DNETHDR>", 9, &length );
    if ( (status&1) == 0 ) return;
    do {
	status = mst_read ( link, buffer, 4096, &length );
	if ( (status&1) == 0 ) return;
	/* Parse out header label */
	for (colon = 0; colon < length; colon++ ) if ( buffer[colon] == ':') {
	    label_len = colon + 5;
	    if ( label_len >= sizeof(label) ) label_len = sizeof(label) - 1;
	    tu_strcpy ( label, "HTTP_" );
	    tu_strnzcpy ( &label[5], buffer, label_len - 5 );
	    tu_strupcase ( label, label );
	    /* Trim leading whitespace */
	    for ( colon++; buffer[colon] == ' ' || buffer[colon] == '\t';
		colon++ );

	    /*
	     * Check for special labels we are interested in.
	     */
	    if ( tu_strncmp ( label, "HTTP_CONTENT_LENGTH", 20 ) == 0 ) {
		tu_strcpy ( label, "CONTENT_LENGTH" );
		LOCK_C_RTL
		*content_length = atoi(&buffer[colon]);
		UNLOCK_C_RTL
	    } else if ( tu_strncmp(label,"HTTP_CONTENT_TYPE", 20) == 0 ) {
		tu_strcpy ( label, "CONTENT_TYPE" );
	    }
	    merge_var ( label, &buffer[colon], length-colon, env );
	    break;
	} else if ( buffer[colon] == '-' ) buffer[colon] = '_';
    } while ( length > 0 ); 	/* continue until nul line read */
}
/********************************************************************/
/* Main service routine, called by shell start routine for newly create service
 * thread.
 */
int cgifork_mst
	( mst_link_t link, char *service, char *info, int ndx, int avail )
{
    char *prolog[5], *argv[20];
    int start, length, length2, argc, i, j, status, xlate_len, ep_len;
    int content_length,cfd,  chan, p[2], pid, io_size, child_status;
    tu_stream proto;
    struct env_buffer env;
    char *inp, *value, *cfile, arg_buf[2048], search_arg[4096], qbuf[4096];
    char exec_path[256], temp[256], bin_path[256], xlate_buf[256], errmsg[256];
    int content;
    /*
     * Read prologue sent by server.
     */
    if ( http_log_level > 0 ) tlog_putlog ( 1, 
	"Service!AZ/!SL connected, info: '!AZ', pool remaining: !SL!/", 
	service, ndx, info, avail );
    j = 0;		/* portion of arg_buf filled */
    for ( i = 0; i < 4; i++ ) {
	prolog[i] = &arg_buf[j];
	status = mst_read ( link, prolog[i], sizeof(arg_buf)-j, &length );
	if ( status != MST_NORMAL ) break;
	arg_buf[j+length] = '\0';
	j = j + length + 1;
    }
    if ( i < 4 ) return 1;
    /*
     * Build environment array.
     */
    env.count = env.used = 0;
    value = getenv ( "PATH" );
    if ( value ) add_var ( "PATH", value, tu_strlen(value), &env );
    value = getenv ( "HOME" );
    if ( value ) add_var ( "HOME", value, tu_strlen(value), &env );
    value = getenv ( "TERM" );
    if ( value ) add_var ( "TERM", value, tu_strlen(value), &env );
    value = getenv ( "USER" );
    if ( value ) add_var ( "USER", value, tu_strlen(value), &env );
    add_var ( "GATEWAY_INTERFACE", "CGI/1.1", 7, &env );
    add_var ( "REQUEST_METHOD", prolog[1], tu_strlen(prolog[1]), &env );
    add_var ( "SERVER_PROTOCOL", prolog[2], tu_strlen(prolog[2]), &env );
    /*
     * The ID2 tag returns information needed for several variables, delimited
     * by spaces.
     */
    status = mst_write ( link, "<DNETID2>", 9, &length );
    if ( (status&1) == 0 ) return status;
    status = mst_read ( link, qbuf, sizeof(qbuf)-1, &length );
    if ( (status&1) == 0 ) return status;
    else {
	/*
	 * Parse result into the separate environment variables.
	 */
	static char *id2_varname[] = { "SERVER_SOFTWARE", "SERVER_NAME",
	    "SERVER_PORT", "REMOTE_PORT", "!REMOTE_ADDR", "REMOTE_USER",
	    "REMOTE_HOST", "" };
	start = 0;
	for ( i = j = 0; i < length; i++ ) if ( qbuf[i] == ' ' ) {
	    char * varname, dot_fmt[16]; int addr;
	    varname = id2_varname[j++];
	    if ( *varname == '!' ) {	/* special formatting */
		varname = &varname[1];
		qbuf[i] = '\0';
		LOCK_C_RTL
		addr = atoi ( &qbuf[start] );
		sprintf ( dot_fmt, "%d.%d.%d.%d", (addr&255),
		    ((addr>>8)&255), ((addr>>16)&255), ((addr>>24)&255) );
		UNLOCK_C_RTL
		add_var ( varname, dot_fmt, tu_strlen(dot_fmt), &env );

	    } else if ( *varname == '?' ) { /* conditional add */
		varname = &varname[1];
		if ( i > start )
	            add_var ( varname, &qbuf[start], i - start, &env );
	    } else {
	        add_var ( varname, &qbuf[start], i - start, &env );
	    }
	    if ( id2_varname[j][0] == '\0' ) break;
	    start = i + 1;
	}
	if ( i == length ) 
	    add_var ( id2_varname[j], &qbuf[start], i - start, &env );
    }
    /*
     * Determine SCRIPT_PATH, SCRIPT_NAME, leave remainder (i.e. PATH_INFO)
     * in inp variable.
     */
    tu_strnzcpy ( exec_path, info, sizeof(exec_path)-2 );
    ep_len = tu_strlen ( exec_path );
    status = mst_write ( link, "<DNETPATH>", 10, &length );
    if ( status == MST_NORMAL )  status = mst_read 
		( link, bin_path, sizeof(bin_path)-1, &length );
    if ( status != MST_NORMAL ) return status;

    bin_path[length] = '\0';		/* terminate string */
    add_var ( "SCRIPT_PATH", bin_path, length, &env );

    inp = prolog[3];
    argv[0] = inp; argc = 1;
    if ( bin_path[0] == '/' ) {
        if ( 0 != tu_strncmp(inp, bin_path, length) ) {
	   tlog_putlog(0,
	       "Invalid URL (!AZ), must start with !AZ!/",prolog[3], bin_path);
	    return 0;
	}
	/*
	 * Determine length of script-name element and append to exec_path
	 */
	for ( i = 0; inp[i+length] && (inp[i+length] != '/'); i++ ) {
	    if ( ep_len < sizeof(exec_path)-2 ) exec_path[ep_len++] =
		inp[i+length];
	}
	exec_path[ep_len] = '\0';
	add_var ( "SCRIPT_NAME", inp, length+i, &env );
	inp = &inp[i+length];
	if ( *inp == '/' ) argv[0] = inp+1;
        j = tu_strlen ( inp );
    } else {
	/*
	 * Assume we are a presentation script, so entire string is path_info.
	 */
	add_var ( "SCRIPT_NAME", exec_path, ep_len, &env );
	   inp = qbuf;
	status = mst_write ( link, "<DNETRQURL>", 11, &length2 );
	if ( (status&1) == 0 ) return status;
	status = mst_read ( link, qbuf, sizeof(qbuf)-1, &length2 );
	if ( (status&1) == 0 ) return status;
	else {
	    /* Trim off search arg */
	   for ( j = 0; j < length2; j++ ) if ( qbuf[j] == '?' ) break; 
	   qbuf[j] = '\0';
	}
    }
    /*
     * Set path_info and path_translated.
     */
    add_var ( "PATH_INFO", inp, j, &env );
    
    if ( (bin_path[0] == '/') ) {
        /*
         * Translate the path.
         */
        status = mst_write ( link, "<DNETXLATE>", 11, &length2 );
        if ( (status&1) == 0 ) { 
	    tlog_putlog(0,"MST I/O failure in cgifork!/"); return status;}
        status = mst_write ( link, inp, j, &length2 );
	status = mst_read ( link, xlate_buf, sizeof(xlate_buf)-1, &xlate_len);
        if ( (status&1) == 0 ) {
	    tlog_putlog(0,"MST I/O failure in cgifork!/"); return status;}
        xlate_buf[xlate_len] = '\0';
        add_var ( "PATH_TRANSLATED", xlate_buf, xlate_len, &env );
    } else {
	/*
	 * The file referenced was included in the prolog.
	 */
	add_var ( "PATH_TRANSLATED", prolog[3], tu_strlen (prolog[3]), &env );
    }
    /*
     * Fetch search argument.  Trim leading '?' in environment value.
     */
    status = mst_write ( link, "<DNETARG2>", 10, &length2 );
    status = mst_read ( link, search_arg, sizeof(search_arg)-1, &length2 );
    if ( (status&1) == 0 ) { 
	tlog_putlog(0,"MST I/O failure in cgifork!/"); return status;
    } else if ( length2 > 0 ) {
	search_arg[length2] = '\0';
	add_var ( "QUERY_STRING", &search_arg[1], length2-1, &env );
    }
    /*
     * If querystring looks like from ISINDEX rather than form (i.e.
     * no '=' in string), split arguments.
     */
    add_var ( "EXEC_PATH",  exec_path, ep_len, &env );
    argv[0] = exec_path; argc = 1;
    for ( i = 1; i < length2; i++ ) if ( search_arg[i] == '=' ) break;
    if ( i >= length2 ) {
	/*
	 *  Breakup search_arg on pluses into separate strings on argv[] list.
	 */
	search_arg[0] = '+';
	for ( i = 0; (i < length2) && (argc < 19); i++ ) {
	    if ( search_arg[i] == '+' ) {
	        search_arg[i] = '\0';
	        argv[argc++] = &search_arg[i+1];
	    }
	}
	/*
	 * Unesape the words on argv.  We know all strings have nulltermination.
	 */
	for (i = 1; i < argc; i++) tu_unescape ( argv[i], argv[i], length2 );
    }
    argv[argc] = (char *) 0;		/* terminate argument list */
    /*
     * Convert request headers to environment variables, then terminate list.
     */
    convert_headers ( link, &env, qbuf, &content_length );
    env.ptr[env.count] = (char *) 0;
    /*
     * Read supplied data into temporary file.
     */
    if ( content_length > 0 ) {
	tu_strcpy ( temp, "cgifork" );
	LOCK_C_RTL
	cfile = tmpnam ( temp );
	UNLOCK_C_RTL
	if ( cfile ) {
	    LOCK_C_RTL
	    cfd = creat ( cfile, 06666 );
	    UNLOCK_C_RTL
	    if ( http_log_level > 5 ) tlog_putlog ( 6, 
			"Temporary file: '!AZ', cfd: !SL!/", cfile, cfd );
	    if ( cfd < 0 ) content_length = -1;
	} else {
	    content_length = -1;
	}
	for ( i = content_length; i > 0; i -= length ) {
	    status = mst_write ( link, "<DNETINPUT>", 11, &length );
	    if ( (status&1) == 0 ) break;;
	    status = mst_read ( link, qbuf, 1024, &length );
	    if ( (status&1) == 0 ) break;;
	    LOCK_C_RTL
	    if ( length > 0 ) write ( cfd, qbuf, length );
	    UNLOCK_C_RTL
	}
    }
#ifdef dump_env
    /*
     * Dump the built array.
     */
    mst_write ( link, "<DNETTEXT>", 10, &length );
    mst_write ( link, "200 environment array follows", 29, &length );
    for ( i = 0; i < env.count; i++ ) 
	mst_write ( link, env.ptr[i], tu_strlen(env.ptr[i]), &length );
    mst_write ( link, "</DNETTEXT>", 11, &length );
    return 1;
#endif
    /*
     * Creat pipe and fork.
     */
    status = mst_write ( link, "<DNETCGI>", 9, &length );
#ifdef VMS
    setup_child_io ( content_length > 0 ? cfile : "", p );
    /* pthread_lock_global_np(); */
    pid = vfork();
    if ( http_log_level > 5 ) tlog_putlog(6,"vfork() result: !XL!/", pid );
    if ( pid == 0 ) {
	/*
	 * We are child process, make our input the content file and
	 * our output the pipe (p[1]).
	 * Chain to requested program using request env.
	 */
        if ( http_log_level > 5 ) tlog_putlog(6,"execing: !AZ, env used: !SL!/", 
		exec_path, env.used );
	execve ( exec_path, argv, env.ptr );
        cleanup_child_io( (content_length > 0) );
	close ( p[0] );
	tlog_putlog(0,"Error returned from execve call !SL!/", errno );
	exit(1);
    }
    /* pthread_unlock_global_np(); */
    cleanup_child_io( (content_length > 0) );
    if ( pid < 0 ) {
	tlog_putlog(0,"fork() error!/" );
	close_mailbox ( p );
	return -1;
    }
    if ( http_log_level > 1 ) tlog_putlog(2,
	"Service!AZ/!SL forked pid: !SL!/", service, ndx, pid );
    /*
     * Relay data from child process to thread that created us.
     */
    if ( content_length > 0 ) close ( cfd );
    io_size = sizeof(proto->data);
    if ( io_size > sizeof(qbuf) ) io_size = sizeof(qbuf);
    for ( status = 1; ; ) {
	length = read_from_child ( p, pid, qbuf, io_size );
	if ( length <= 0 ) break;
	status = mst_write ( link, qbuf, length, &length2 );
	if ( (status&1) == 0 ) break;
    }
    /*
     * cleanup, shutdown pipe and signal output done.
     */
    close_mailbox ( p );
    if ( content_length > 0 ) delete ( cfile );

#else  /* !VMS - OSF */

    if ( pipe(p) < 0 ) { 
	tlog_putlog(0,"Error creating pipe for fork()!/");
	return 0;
    }
    pid = fork();
    if ( pid == 0 ) {
	/*
	 * We are child process, make our input the content file and
	 * our output the pipe (p[1]).
	 */
	close ( p[0] );
	if ( content_length > 0 ) {
	    lseek ( cfd, 0, 0 );
	    dup2 ( cfd, fileno(stdin) );
	}
	dup2 ( p[1], fileno(stdout) );
	/*
	 * Set all descriptors higher than 2 to close-on-exec
	 */
	for ( i = 3; i < cgifork_fdtablesize; i++ ) {
	    int flags;
	    flags = fcntl ( i, F_GETFD );
	    if ( (flags != -1) && !(flags&FD_CLOEXEC) ) {
		fcntl ( i, F_SETFD, flags | FD_CLOEXEC );
	    }
	}
	/*
	 * Chain to requested program using request env.
	 */
	setuid ( getuid() );		/* make us root */
	setuid ( http_saved_uid );		/* Make us web server */
	if ( -1 == execve ( exec_path, argv, env.ptr ) ) {
	    /* If arglist too long, retry with smaller */
	    argv[1] = (char *) 0;	/* kill argument list */
	    if ( errno == E2BIG ) execve ( exec_path, argv, env.ptr );
	}
	close ( p[1] );
	tlog_putlog(0,"Error returned from execve call !SL!/", errno );
	exit(1);
    }
    close ( p[1] );
    if ( pid < 0 ) {
	tlog_putlog(0,"fork() error!/" );
	close ( p[0] );
	return -1;
    }
    if ( http_log_level > 1 ) tlog_putlog(2,"forked pid: !SL!/", pid );
    /*
     * Relay data from child process to thread that created us.
     */
    if ( content_length > 0 ) close ( cfd );
    io_size = sizeof(proto->data);
    if ( io_size > sizeof(qbuf) ) io_size = sizeof(qbuf);
    for ( status = 1; ; ) {
	length = read ( p[0], qbuf, io_size );
	if ( length <= 0 ) break;
	status = mst_write ( link, qbuf, length, &length2 );
	if ( (status&1) == 0 ) break;
    }
    /*
     * cleanup, shutdown pipe and signal output done.
     */
    close ( p[0] );
    waitpid ( pid, &child_status, 0 );
    if ( content_length > 0 ) unlink ( cfile );
#endif  /* VMS */
    if ((status&1) == 1) status = mst_write (link, "</DNETCGI>", 10, &length);

    mst_close ( link );
    return 1;
}
#ifdef VMS
/****************************************************************************/
/* The follow support routines enable us to redirect standard input and output
 * of the fork/exec'ed child to the content file and pipe.  This is done
 * by defining user-mode logicals for sys$input and sys$Output.
 *
 * Defining SYS$INPUT breaks the operation of execve, so define ht
 * logical CGI_CONTENT_FILE instead that the script can open to get the
 * form contents.
 */
static int setup_child_io ( char *cfile, int *p )
{
    int status, length, iosb[2];
    struct { short length, code; char *buffer; int *retlen; } item[2];
    char *tmp, pname[256];
    /*
     * Create mailbox and fetch the name.
     */
    p[0] = p[1] = 0;
    status = sys$crembx ( 0, p, 512, 512, 0x0ff00, 0, 0, CMB$M_READONLY );
    if ( (status&1) == 0 ) return status;
    item[0].length = 64;
    item[0].code = DVI$_DEVNAM;
    item[0].buffer = pname;
    item[0].retlen = &length; length = 0;
    item[1].length = item[1].code = 0;		/* terminate list */
    status = sys$getdviw ( 0, p[0], 0, &item, iosb, 0, 0, 0 );
    if ( (status&1) == 0 ) return status;
    pname[length] = '\0';
    /*
     * Initialize item list for $CRELNM call and create sys$input logical.
     */
    item[0].length = tu_strlen ( cfile );
    item[0].code = LNM$_STRING;
    item[0].buffer = cfile;
    item[0].retlen = (int *) 0;
    item[1].length = item[1].code = 0;		/* terminate list */
    if ( item[0].length > 0 ) {
	status = sys$crelnm ( 0, &log_table, &sysin, 0, &item );
    }
    /*
     * Get device name for output file (pipe).
     */
    if ( *pname ) {
	/*
	 * Name obtained, define sys$Output with this name.
	 */
        item[0].buffer = pname;
	item[0].length = tu_strlen ( item[0].buffer );
	pname[item[0].length] = '\0';
	status = sys$crelnm ( 0, &log_table, &sysout, 0, &item );
    } else status = 20;
    p[1] = (status&1);
    return status;
}

static int close_mailbox ( int *p )
{
    if ( p[1] < 0 ) return 1;
    p[1] = -1;
    return sys$dassgn ( p[0] );
}
/*
 * Delete logical names created by setup_child_io().
 */
static int cleanup_child_io(int input)
{
    int status;
    if ( input ) {
	status = sys$dellnm ( &log_table, &sysin, 0 );
    }
    status = sys$dellnm ( &log_table, &sysout, 0 );
    return status;
}
/*
 * Read from mailbox, discard EOF's.
 */
static int read_from_child ( int *p, int pid, char *buffer, int bufsize )
{
    int status, func;
    struct { short status, length; int pid; } iosb;

    for ( ; ; ) {
	func = IO$_READVBLK | ( p[1] == 1 ? IO$M_WRITERCHECK : 0 );
        status = sys$qiow ( 0, p[0], func, &iosb, 0, 0, 
		buffer, bufsize <= 512 ? bufsize-1 : 511, 0, 0, 0, 0 );
       if ( http_log_level > 12 ) tlog_putlog ( 11, 
		"mbx read status: !SL/!SL l: !SL pid: !XL!/", status,
		iosb.status, iosb.length, iosb.pid );
	if ( (status & 1) == 0 ) return -1;
	if ( (iosb.status&1) == 1 ) {
	    /*
	     * Only return record if from requested PID
	     */
	    buffer[iosb.length] = '\n';		/* implied record delimiter */
	    if ( iosb.pid == pid ) {
		p[1] = 1;
		return iosb.length+1;
	    }
	} else if ( iosb.status != SS$_ENDOFFILE ) {
	    /*
	     * Pipe is 'broken'
	     */
	    return -1;
	}
    }
    return 0;
}
#endif /* VMS */
