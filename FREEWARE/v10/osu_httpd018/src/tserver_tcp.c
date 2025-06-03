/*
 * This pthread module creates a thread that listens on a specified TCP port
 * for incoming connections.  Each connection received creates a new thread
 * to handle the session.  You may call ts_declare_port multiple times to
 * listen on more than one TCP port.  On thread exit, the connection is
 * closed.
 *
 * If a connection is received and no client contexts are available, the
 * listener thread will wait for up to three seconds for a context
 * to free up.
 *
 * int ts_declare_tcp_port ( port_num, client_limit, attr, thread, start );
 *	int start ( ctx, port, remote_address, index, remaining );
 * int ts_set_manage_port ( port_num, host_address, callback );
 *	int callback ( ctx, port, shutdown_time );
 * int ts_set_logging ( callback );
 *	int callback ( min_level, faostr, ... );
 * int ts_tcp_write ( ctx, buffer, length );
 * int ts_tcp_read ( ctx, buffer, maxlen, *length );
 * int ts_tcp_close ( ctx );
 * int ts_tcp_info ( local_port, remote_port, remote_address, remote_host );
 * char *ts_tcp_remote_host();
 * int ts_set_local_addr ( address );
 *
 * Revised: 26-MAR-1994		Added CMU IP support.
 * Revised:  7-JUN-1994		Removed AST_SAFE stuff, DEC documented
 *				the cond_signal_int behaviour.
 * Revised: 20-JUL-1994		Cleanup use of startup_status.
 * Revised: 22-JUL-1994		Add code to stall for connection if no clients.
 * Revised: 23-JUL-1994		Added ts_tcp_remote_host() function (UCX only).
 * Revised:  8-AUG-1994		Fixed bug in ts_tcp_remote_host(), accvio on
 *				lookup failure.
 * Revised:  8-AUG-1994		Fixed bug in ts_tcp_remote_host(), missing arg.
 *				tu_strnzcpy call, CMUTCP section.
 * Revised: 15-AUG-1994		Added support for TCPWARE interface.
 * Revised: 20-AUG-1994		Added manage port.
 * Revised: 23-AUG-1994		Work around Multinet include file bugs.
 * Revised: 24-SEP-1994		Remove direct reference to tlog_putlog.
 * Revised:  3-NOV-1994		Add keepalive option to UCX sockets.
 * Revised: 12-JAN-1995		Add George Carrette's (gjc@village.com) mods
 *				to support multihomed hosts.
 * Revised: 3-MAR-1995		Try to make CMUTCP variant of accept_connect()
 *				more robust.
 */
#include "pthread_np.h"
#include "tutil.h"

/* Default to use UCX */
#if !defined(CMUTCP) && !defined(TWGTCP) && !defined(MULTINET)
#if !defined(UCXTCP) && !defined(TCPWARE)
#define UCXTCP
#endif
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <descrip.h>
#include <iodef.h>
int SYS$DASSGN();

#ifdef UCXTCP
#define TCPSEL(ucx,mlt,twg,cmu,twr) ucx
#include <UCX$INETDEF.H>
#define sockaddr_in SOCKADDRIN
#endif
#ifdef MULTINET
#define TCPSEL(ucx,mlt,twg,cmu,twr) mlt
#include "MULTINET_ROOT:[MULTINET.INCLUDE.SYS]TYPES.H"
#include "MULTINET_ROOT:[MULTINET.INCLUDE.NETINET]IN.H"
#include "MULTINET_ROOT:[MULTINET.INCLUDE.SYS]SOCKET.H"
#include "MULTINET_ROOT:[MULTINET.INCLUDE.VMS]INETIODEF.H"
#include "MULTINET_ROOT:[MULTINET.INCLUDE]NETDB.H"
#include "MULTINET_ROOT:[MULTINET.INCLUDE]errno.h"
#endif
#ifdef TWGTCP
#define TCPSEL(ucx,mlt,twg,cmu,twr) twg
#include "TWG$COMMON:[NETDIST.INCLUDE.SYS]TYPES.H"
#include "TWG$COMMON:[NETDIST.INCLUDE.SYS]NETDB.H"
#include "TWG$COMMON:[NETDIST.INCLUDE.SYS]SOCKET.H"
#include "TWG$COMMON:[NETDIST.INCLUDE.NETINET]IN.H"
#include "TWG$COMMON:[NETDIST.INCLUDE.VMS]INETIODEF.H"
#endif
#ifdef TCPWARE
#define TCPSEL(ucx,mlt,twg,cmu,twr) twr
#include "tcpip_include:types.h"
#include "tcpip_include:in.h"
#include "tcpip_include:socket.h"
#include "tcpip_include:inet.h"
#include "tcpip_include:inetiodef.h"
#include "tcpip_include:netdb.h"
#include "tcpip_include:sockerr.h"
#endif
#ifdef CMUTCP
#define TCPSEL(ucx,mlt,twg,cmu,twr) cmu
#include <socket.h>
#include <in.h>
#define CMU_LISTEN_ADDRESS "0.0.0.0"
#define CMU_LISTEN_FLAGS 0
#define CMU_CLOSE_FLAGS 0
#define CMU_WRITE_FLAGS 0
#else
#define CMU_WRITE_FLAGS 0
#endif
#ifndef EPERM
#include <errno.h>
#endif
/* we have our own macros because we may not be able to trust
   the reentrancy of all vendor-supplied helper functions */

#define HTONS(_x) (((_x)/256)&255) | (((_x)&255)<<8);

#define ERROR_FAIL(s,text) if (s == -1) perror(text);
#define PORT_THREAD_STACK 6000

typedef struct client_context client_ctx_struct, *client_ctx;
struct client_context {
    struct port_context *flink, *blink;
    int index, port_num;
    pthread_t thread;
    int status;
    int *stack_top;		/* Top address (nearly) of thread's stack */
    pthread_cond_t io_done;
    int (*start)( client_ctx ctx, 
	  short port, unsigned char *rem_addr,
	  int ndx, int length);	/* Application supplied startup routine */
    short chan;			/* Network channel */
    short dns_chan;		/* Channel for name lookups or -1 */
    int clients_remaining;
    short iosb[4];

    struct sockaddr_in remote_address;     /* Remote address */
    int hn_status;		/* Status of hostname item: 0, 1, -1 */
    char hostname[256];		/* Remote host name */
};

struct port_context {
    struct port_context *flink, *blink;
    int port_num;
    int status;
    pthread_t thread;
    int connect_pending;		/* Connection is pending */
    pthread_cond_t client_available;	/* signals freelist is non-empty */
    pthread_attr_t *client_attr;	/* Attributes for created clients */
    short chan, fill;
    int (*start)( client_ctx ctx, 
	  short port, unsigned char *rem_addr,
	  int ndx, int length);	/* Application supplied startup routine */
    int client_count, client_limit;
    client_ctx_struct client_list;
    client_ctx free_clients;
};
typedef struct port_context port_ctx_struct, *port_ctx;
#define ts_client_ctx client_ctx
#include "tserver_tcp.h"		/* validate header file */

static port_ctx_struct port_list;
static int ts_startup_client ( client_ctx ctx );
port_ctx tserver_tcp_port_list;
#define CB_SIZE 128
static struct hn_cache_block {
    unsigned long address;
    int status;
    char hostname[256];
} cb[CB_SIZE];
static ts_manage_port;		/* Remote port number to induce shutdowns */
static unsigned int ts_manage_host;/* Remote address to induce port shutdown */
static int (*ts_manage_callback)(ts_client_ctx ctx, short port, int
		*shutdown_time);  /* upcall for management connects */
static int ts_logging = 0;
static int (*ts_putlog)(int, char *, ...);	/* upcall for logging */
static unsigned long local_ip_addr = 0;	/* local listen address, stored in 
				network byte order */
/*
 * The following mutexes and conditions serialize access to resources:
 *    tcp_io		This mutex is used before SYS$QIO calls. 
 *			Thread-specific condition variables are then used
 *			in conjunction with an AST to wait on the I/O.
 *    tcp_ctl		This mutex is obtained when a thread needs to modify
 *			the client list or port list data structures.
 *    port_startup	Mutex to serialize access to port startup/init routine.
 *    port_startup_done	Condition used in conjuction with port_startup.
 */
static pthread_mutex_t tcp_io;		/* tcp/io data mutex */
static pthread_mutex_t tcp_ctl;		/* tcp context database mutex */
static pthread_mutex_t port_startup;	/* serialize port startup */
static pthread_cond_t port_startup_done;  /* condition */
static int startup_status;		/* Predicate */

static pthread_key_t port_key;		/* Private data for port listener */
static pthread_key_t client_key;	/* Private data for TCP client */
static pthread_once_t ts_mutex_setup = pthread_once_init;

static void ts_init_mutexes();		/* forward reference */
static int ts_synch_ast(), ts_synch_io();
static int create_listen_socket(int, short, pthread_cond_t *);
typedef struct { int length; struct sockaddr_in a; } socket_info;
static int accept_connect ( int, int, int, pthread_cond_t *, socket_info * );
static int port_manage ( port_ctx, int, socket_info * );
/*************************************************************************/
/* Set flag and putlog callback routine address.  Note that this routine
 * must only be called if the tcp_ctl mutex is owned by the current thread.
 */
int ts_set_logging ( int callback(int, char *, ...) )
{
    int previous_logging;
    /*
     * Initialize globals for module, only once though.
     */
    pthread_once ( &ts_mutex_setup, ts_init_mutexes );
    /*
     * Get mutex and update shared data.  Return value if previous setting.
     */
    pthread_mutex_lock ( &tcp_ctl );
    previous_logging = ts_logging;
    ts_putlog = callback;
    if ( ts_putlog ) ts_logging = 1; else ts_logging = 0;
    pthread_mutex_unlock ( &tcp_ctl );
    return previous_logging;
}
/*
 * Use this macro with care since it leaves an if dangling!
 */
#define PUTLOG if ( ts_logging ) (*ts_putlog)
/*************************************************************************/
/* Set local listen address to bind sockets to.  Note that local_ip_addr
 * must be in local byte order.   Address must be numeric form.
 */
int ts_set_local_addr ( char *addr ) 
{
    unsigned long a[4], *address;
    unsigned char octet[sizeof(unsigned long)];
    int j;
    /*
     * Initialize globals for module, only once though.
     */
    pthread_once ( &ts_mutex_setup, ts_init_mutexes );
    /*
     * Decode address, IP addresses are defined as 4 octets so it is
     * already in network data order when cast to a long.
     */
    LOCK_C_RTL
    memset(a,0,sizeof(a));
    sscanf(addr,"%d.%d.%d.%d",&a[0],&a[1],&a[2],&a[3]);
    UNLOCK_C_RTL
    for ( j = 0; j < 4; j++ ) octet[j] = a[j];
    address = (unsigned long *) octet;
    /*
     * Get mutex and update shared data.
     */
    pthread_mutex_lock ( &tcp_ctl );
    local_ip_addr = *address;
    pthread_mutex_unlock ( &tcp_ctl );
    PUTLOG ( 5, "Address !AZ encodes as !XL!/", addr, local_ip_addr );
    return 1;			/* Always return success */
}
/*************************************************************************/
/* Initialize port management parameters.  Connects from specified port
 * and address will be handled by callback routine rather than creating
 * a normal client thread.
 */
int ts_set_manage_port ( int port_num, unsigned int host_address,
	int callback(client_ctx ctx, short port, int *shutdown_time) )
{
    /*
     * Initialize globals for module, only once though.
     */
    pthread_once ( &ts_mutex_setup, ts_init_mutexes );
    /*
     * Get mutex and update shared data.  Convert port number to network
     * data order so we can compare it directly to remote sockaddr.
     */
    pthread_mutex_lock ( &tcp_ctl );
    ts_manage_port = HTONS(port_num);
    ts_manage_host = host_address;
    ts_manage_callback = callback;
    pthread_mutex_unlock ( &tcp_ctl );
    return 1;
}
/*************************************************************************/
/* Create thread that listens on a particular TCP/IP port and starts
 * new thread for each accepted connection.  A caller supplied start routine
 * is called by the created thread to process the session.
 *
 * This routine can be called multiple times within an application to listen
 * on different ports.  Each port gets it's own client limit.
 *
 * User start (session) routine:
 *	int start ( void *ctx, int port_num, sockaddrin remote );
 */
int ts_declare_tcp_port ( 
    int port_num, 		/* Port number to listen on, host order */
    int client_limit, 		/* Limit on concurrent client threads allowed */
    pthread_attr_t *client_attr, /* Thread attributes for client threads */
    pthread_t *control_thread, 	/* Thread that listens for connects */
    int (*start)( client_ctx ctx, short port, unsigned char *rem_addr,
		int ndx, int length) )	/* Start routine for new clients. */
{
    int ts_startup_port(), stacksize, status, pthread_create();
    port_ctx ctx;
    pthread_attr_t port_attr;
    /*
     * Initialize globals for module, only once though.
     */
    pthread_once ( &ts_mutex_setup, ts_init_mutexes );
    /*
     * Allocate port control block that becomes the thread-specific context
     * data for the new thread.
     */
    LOCK_C_RTL
    ctx = (port_ctx) malloc ( sizeof(port_ctx_struct) );
    UNLOCK_C_RTL
    /*
     * Link the control block into the global list, use mutex to synchronize
     * update of pointers.
     */
    pthread_mutex_lock ( &tcp_ctl );
    ctx->port_num = port_num;
    ctx->flink = &port_list;
    ctx->blink = port_list.blink;
    port_list.blink->flink = ctx;
    port_list.blink = ctx;
    pthread_mutex_unlock   ( &tcp_ctl );
    /*
     * Zero pertinent fields in control block.
     */
    ctx->status = 0;
    ctx->start = start;
    ctx->client_attr = client_attr;
    ctx->client_count = 0;
    ctx->client_limit = client_limit;
    /*
     * Create thread attributes object for the listener thread.  Use FIFO
     * scheduling which will give it a higher priority than the client
     * threads which have round robin scheduling.
     */                                                              
    status = pthread_attr_create ( &port_attr );
    ERROR_FAIL(status,"Port thread attribute create failure" )
    status = pthread_attr_setinheritsched ( &port_attr, PTHREAD_DEFAULT_SCHED );
    status = pthread_attr_setsched ( &port_attr, SCHED_FIFO );
    status = pthread_attr_setprio ( &port_attr, (PRI_FIFO_MIN+PRI_FIFO_MAX)/2 );
    status = pthread_attr_setstacksize ( &port_attr, PORT_THREAD_STACK );

    /* 
     * Create thread and block until it finishes initializing so we can
     * check it's status.
     */
    status = pthread_mutex_lock ( &port_startup );
    ERROR_FAIL(status,"Port startup lock failure" )
    startup_status = 0;

    status = pthread_create (control_thread, port_attr, ts_startup_port, ctx);
    ERROR_FAIL(status,"Port thread create failure" )

    while ( startup_status == 0 ) {
        status = pthread_cond_wait ( &port_startup_done, &port_startup );
        ERROR_FAIL(status,"Port startup wait failure" )
    }
    pthread_mutex_unlock ( &port_startup );

    return ctx->status;
}
/**************************************************************************/
/* Write data to thread's TCP/IP connection.
 */
int ts_tcp_write 
    ( client_ctx ctx,		/* Context passed to port start() function */
    char *buffer,		/* Data buffer to write */
    int length )		/* Number of chars to write */
{
    int SYS$QIO(), status, ts_synch_ast(), ts_synch_io(), remaining, seg;
    /*
     * Make $QIO call and block until AST signals us.
     */
    status = 1;
    for ( remaining = length; remaining > 0; remaining -= seg ) {
	/*
	 * Only write up to 1 K at a time.
	 */
	seg = remaining > 1024 ? 1024 : remaining;
        pthread_mutex_lock ( &tcp_io );
        status = SYS$QIO ( 0, ctx->chan, IO$_WRITEVBLK, &ctx->iosb,
	    ts_synch_ast, &ctx->io_done, buffer, seg, 0, CMU_WRITE_FLAGS, 
	    0, 0 );
	status = ts_synch_io ( status, &ctx->io_done, &ctx->iosb[0] );
        if ( (status&1) == 0 ) break;

#ifndef CMUTCP
	seg = ctx->iosb[1];	/* length actually sent */
#endif
	buffer = &buffer[seg];   /* Point to next unsent char */
    }
    return status;
}
/**************************************************************************/
/* Read data from thread's TCP/IP connection.
 */
int ts_tcp_read
    ( client_ctx ctx,		/* Context passed to port start() function */
    char *buffer,		/* Data buffer to write */
    int maxlen,			/* Size of buffer */
    int *length )		/* Number of chars to write */
{
    int SYS$QIO(), status, ts_synch_ast(), ts_synch_io();
    /*
     * Make $QIO call and block until AST signals us.
     */
    status = 1;
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, ctx->chan, IO$_READVBLK, &ctx->iosb,
	    ts_synch_ast, &ctx->io_done, buffer, maxlen, 0, 0, 0, 0 );
    status = ts_synch_io ( status, &ctx->io_done, &ctx->iosb[0] );
    if ( (status&1) == 1 ) {
	/*
	 * Return length read to caller, some TCP packages treat EOF as
	 * a successful zero-length read, convert these cases.
	 */
	*length = ctx->iosb[1];
#if defined(TWGTCP) || defined(MULTINET) || defined(TCPWARE)
	if ( *length == 0 ) status = 2160;
#endif
    } else *length = 0;
    return status;
}
/**************************************************************************/
/* Perform synchronous disconnect of socket.  Do not deassign channel.
 * THe WIN/TCP and MULTINET interface does not have an explicit close 
 * other than deassigning the channel, so do nothing for that case.
 */
int ts_tcp_close
    ( client_ctx ctx )		/* Context passed to port start() function */
{
    int SYS$QIO(), status, ts_synch_ast(), ts_synch_io();
    /*
     * Make $QIO call and block until AST signals us.
     */
    status = 1;
#ifdef UCXTCP
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, ctx->chan, IO$_DEACCESS, &ctx->iosb,
	    ts_synch_ast, &ctx->io_done, 0, 0, 0, 0, 0, 0 );
    status = ts_synch_io ( status, &ctx->io_done, &ctx->iosb[0] );
#endif
#ifdef CMUTCP
    /* Use _DELETE function to gracefully close connection, sending
     * all data. */
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, ctx->chan, IO$_DELETE, &ctx->iosb,
	    ts_synch_ast, &ctx->io_done, CMU_CLOSE_FLAGS, 0, 0, 0, 0, 0 );
    status = ts_synch_io ( status, &ctx->io_done, &ctx->iosb[0] );
#endif
    return status;
}
/**************************************************************************/
/* Do one-time initialization of static variables for this module, primarily
 * mutexes, condition variables and thread-specific context keys.
 */
static void port_rundown(), client_rundown();
static void ts_init_mutexes ( )
{
    int status, i;
    /*
     * Create mutexes and condition variables.
     */
    status = pthread_mutex_init ( &tcp_io, pthread_mutexattr_default );
    ERROR_FAIL(status,"Error creating tcp_io mutex" )

    status = pthread_mutex_init ( &tcp_ctl, pthread_mutexattr_default );
    ERROR_FAIL(status,"Error creating tcp_ctl mutex" )

    status = pthread_mutex_init ( &port_startup, pthread_mutexattr_default );
    ERROR_FAIL(status,"Error creating port_startup mutex" )

    status = pthread_cond_init ( &port_startup_done, pthread_condattr_default );
    ERROR_FAIL(status,"Error creating port_startup_done condition variable" )
    /* 
     * Create keys used to locate context blocks 
     */
    pthread_keycreate ( &port_key, port_rundown );
    ERROR_FAIL(status,"Error creating port listener context key" )

    pthread_keycreate ( &client_key, client_rundown );
    ERROR_FAIL(status,"Error creating tcp client context key" )
    /*
     * Initialize list head of port contexts created.  The pointers in
     * this structure are protected by the tcp_ctl mutex.
     */
    tserver_tcp_port_list = &port_list;		/* set global pointer */
    pthread_mutex_lock ( &tcp_ctl );
    port_list.flink = &port_list; port_list.blink = &port_list;
    port_list.port_num = 0;	/* flag list header */
    pthread_mutex_unlock ( &tcp_ctl );
    /*
     * Initialize cache for name lookups and manage port parameters.
     */
    for ( i = 0; i < CB_SIZE; i++ ) cb[i].status = 0;
    ts_manage_port = -1;
    ts_manage_host = 0;
}
/***************************************************************************/
/* Define VMS-specific routines to let us call system service routines
 * asynchronously.  Use ts_synch_ast as the ast argument to the system service
 * and a condition variable as the ast parameter.  The ts_synch_io routine
 * then blocks the thread waiting for the condition to be signalled.
 *
 * Note that ts_synch_io assumes tcp_io lock is held by the thread.  It is
 * released upon return.
 */
static int ts_synch_ast ( pthread_cond_t *done )
{
    int delta[2], SYS$SETIMR(), status;
    status = pthread_cond_signal_int_np ( done );

    return status;
}
static int ts_synch_io ( int status, pthread_cond_t *done, short *iosb )
{
    /*
     * Only do wait if system service status OK.
     */
    if ( (status&1) == 1 ) {
        /*
         * Loop until predicate (iosb nozero) is true.
         */
	do {
	    status = pthread_cond_wait ( done, &tcp_io );
	    if ( status == -1 ) break;
        } while ( *iosb == 0 );

	status = *iosb;
#ifdef CMUTCP
	if ( status == 44 ) {		/* abort status */
	    int *ptr;
	    ptr = (int *) iosb;
	    status = ptr[1];		/* IPACP status for abort */
	}
#endif
#ifdef SHOW_ERROR
	if ( status < 0 ) printf("synch_io has negative status: %d\n", status );
#endif
    }
    pthread_mutex_unlock ( &tcp_io );
    return status;
}
/***********************************************************************/
/*
 * Top level of thread for listening on ports  Upon entry, the calling
 * thread will be waiting on the port_startup_done condition.
 */
int ts_startup_port ( port_ctx ctx )
{
    int status, stacksize, i, SYS$ASSIGN(), SYS$QIO();
    int client_chan, SYS$ASSIGN();
    $DESCRIPTOR(tcp_device,TCPSEL("UCX$DEVICE","INET0:","INET:","IP:","INET0"));
    socket_info remote;
    pthread_cond_t io_done;
    client_ctx new_client;
    static int marker = 0;
    /*
     * Create context key for this thread and init to our context block address.
     * The port rundown routine can then cleanup.
     */
    pthread_setspecific ( port_key, ctx );
    ctx->thread = pthread_self();
    ctx->client_list.flink = (port_ctx) &ctx->client_list;
    ctx->client_list.blink = (port_ctx) &ctx->client_list;
    ctx->client_list.index = 0;
    ctx->connect_pending = 0;
    status = pthread_cond_init 
		( &ctx->client_available, pthread_condattr_default );
    ERROR_FAIL(status,"Error creating port_startup_done condition variable" )
    /*
     * Pre-allocate client_limit number client contexts for the client threads.
     */
    ctx->free_clients = (client_ctx) 0;
    LOCK_C_RTL
    for ( i = 0; i < ctx->client_limit; i++ ) {
	client_ctx new;
	new = (client_ctx) malloc ( sizeof(client_ctx_struct) );
	new->index = marker+(ctx->client_limit-i);
	new->dns_chan = -1;
	new->hn_status = 0;
	new->flink = (port_ctx) ctx->free_clients;
	ctx->free_clients = new;
    }
    marker += ctx->client_limit;	/* base for thread index numbers */
    UNLOCK_C_RTL
    /*
     * Assign control channel for listens.
     */
    ctx->chan = 0;
    ctx->status = SYS$ASSIGN ( &tcp_device, &ctx->chan, 0, 0 );
    if ( (ctx->status&1) == 1 ) {
        /*
	 * We found, device, create condition to synchronize I/O
	 */
	status = pthread_cond_init ( &io_done, pthread_condattr_default );
	/*
	 * Create socket and bind address that we will listen on.
	 */
	ctx->status = create_listen_socket 
		(  ctx->port_num, ctx->chan, &io_done );
    }
    else return status;
    /* 
     * Acquire startup mutex so we know creating thread is waiting on us, then
     * set status variable and signal that setup is done 
     */
    status = pthread_mutex_lock ( &port_startup );
    if ( status == -1 ) perror ( "port_startup thread mutex" );
    startup_status = ctx->status;
    status = pthread_mutex_unlock ( &port_startup );
    status = pthread_cond_signal ( &port_startup_done );
    ctx->status = 0;		/* No longer starting */
    /*
     * Now  listen for incoming connections.
     */
    for ( ; ; ) {
	int length, current_port;
	/*
	 * Assign channel and listen for connect on newly assigned channel.
	 */
        client_chan = 0;
	status = SYS$ASSIGN ( &tcp_device, &client_chan, 0, 0 );
	if ( (status&1) == 0 ) break;
	status = accept_connect ( ctx->port_num, ctx->chan, 
		client_chan, &io_done, &remote );
	if ( (status&1) == 0 ) break;
	/*
	 * See if connect is from privileged management port.  Note that 
	 * port number is in network data order.  Since ts_manage_port is
	 * a shared variable, get mutex before examinging it.
	 */
	current_port = remote.a.TCPSEL(SIN$W_PORT,sin_port,sin_port,
		sin_port, sin_port);
	pthread_mutex_lock ( &tcp_ctl );
	if ( current_port == ts_manage_port ) {
	    /*
	     * Only accept from proper host.
	     */
	    unsigned int current_address;
	    current_address = remote.a.TCPSEL(SIN$L_ADDR,sin_addr.s_addr,
		sin_addr.s_addr,sin_addr.s_addr,sin_addr.s_addr);
	    if ( current_address == ts_manage_host ) {
		/*
		 * If port_manage returns a non-zero value, exit loop with that
		 * status.  Close client connection in all cases afterward.
		 */
		pthread_mutex_unlock ( &tcp_ctl );
		status = port_manage ( ctx, client_chan, &remote );
	        SYS$DASSGN ( client_chan );
		if ( status ) break;
		continue;
	    }
	}
	/*
	 * Allocate client context block from free list.  Grab mutex while
	 * while playing with module-wide data structures.
	 */
	new_client = ctx->free_clients;
	if ( ! new_client ) {
	    /*
	     * Give ourselves second chance to get client block, waiting
	     * up to 3 seconds. 
	     */
	    struct timespec delta, abstime;
	    delta.tv_sec = 3;
	    delta.tv_nsec = 0;
	    if ( 0 == pthread_get_expiration_np ( &delta, &abstime ) ) {
		PUTLOG ( 0, "Thread list exhausted on port !SL, !%D!/", 
			ctx->port_num, 0 );
		ctx->connect_pending = 1;
	        pthread_cond_timedwait 
		    ( &ctx->client_available, &tcp_ctl, &abstime );
		ctx->connect_pending = 0;
		new_client = ctx->free_clients;
	    } else PUTLOG ( 0, "Time computation problem!/" );
	}

	if ( new_client ) {
	    /*
	     * We got block, initialize it.
	     */
	    ctx->free_clients = (client_ctx) new_client->flink;
	    ctx->client_count++;
	    new_client->flink = (port_ctx) &ctx->client_list;
	    new_client->blink = ctx->client_list.blink;
	    ctx->client_list.blink = (port_ctx) new_client;
	    new_client->blink->flink = (port_ctx) new_client;
	    new_client->status = 0;
	    new_client->hn_status = 0;		/* host name unknown */
	    new_client->port_num = ctx->port_num;
	    new_client->clients_remaining = 
			ctx->client_limit - ctx->client_count;
	}
	pthread_mutex_unlock ( &tcp_ctl );
	/*
	 * Start new thread to process request if we got a client block.
	 */
	if ( new_client ) {
	    int pthread_create();
	    new_client->chan = client_chan;
	    new_client->start = ctx->start;
	    new_client->remote_address = remote.a;

	    status = pthread_cond_init 
			( &new_client->io_done, pthread_condattr_default );

    	    status = pthread_create ( &new_client->thread, *(ctx->client_attr),
		ts_startup_client, new_client );
	    if ( status == -1 ) perror ( "error creating client thread" );
	} else {
	    pthread_mutex_lock ( &tcp_ctl );
	    PUTLOG ( 0, "Connect reject, no free client contexts!/" );
	    pthread_mutex_unlock ( &tcp_ctl );
	    SYS$DASSGN ( client_chan );
	}
    }
    /* Cleanup resources and return final status */
    pthread_cond_destroy ( &io_done );
    return status;
}

/***************************************************************************/
/* Handle management commands.  Read int value from remote client and
 * use that as timeout value for rundown.
 */
static int port_manage ( port_ctx ctx, int client_chan, socket_info *remote )
{
    int status, length, count, shutdown_time;
    struct timespec delta, abstime;
    struct client_context mgr;
    char number[16];
    /*
     * Make dummy client context so callback can use this modules read
     * and write functions.
     */
    mgr.chan = client_chan;
    status = pthread_cond_init ( &mgr.io_done, pthread_condattr_default );
    /*
     * Have manage callback process the request.
     */
    status = (*ts_manage_callback) ( &mgr, ctx->port_num, &shutdown_time );
    /*
     * Any non-zero status returned by callback causes shutdown.
     */
    if ( status ) {
	/*
	 * Wait indicated time for all pending action to run down.
	 */
	delta.tv_sec = shutdown_time;
	delta.tv_nsec = 0;
	if ( 0 != pthread_get_expiration_np(&delta, &abstime) ) return status;
	/*
	 * Wait for existing clients to rundown.
	 */
	pthread_mutex_lock ( &tcp_ctl );
	PUTLOG ( 1, "shutting down TCP listenport !SL!/", ctx->port_num );
	count = ctx->client_count;		/* initial active */
	while ( ctx->client_count > 0 ) {
	    ctx->connect_pending = 1;
	    if ( 0 > pthread_cond_timedwait 
		    ( &ctx->client_available, &tcp_ctl, &abstime ) ) break;
	}
	ctx->connect_pending = 0;
	pthread_mutex_unlock ( &tcp_ctl );
	/*
	 * Write back number we randown and close connection.
	 */
	tu_strint ( count, number );
	ts_tcp_write ( &mgr, number, tu_strlen(number) );
	ts_tcp_close ( &mgr );
    }
    pthread_cond_destroy ( &mgr.io_done );
    return status;
}
/***************************************************************************/
/* Issue QIO to accept connection.
 */
static int accept_connect ( int port_num, int listen_chan, int client_chan,
	pthread_cond_t *io_cond, socket_info *remote )
{
    int status, SYS$QIO();
    struct { short status, count, d1, d2; } iosb;
#ifdef TWGTCP
        pthread_mutex_lock ( &tcp_io );
	status = SYS$QIO ( 0, client_chan, IO$_ACCEPT, &iosb,
		ts_synch_ast, io_cond, 
		remote, sizeof(socket_info), listen_chan, 0, 0, 0 );
	status = ts_synch_io ( status, io_cond, &iosb.status );
#endif
#ifdef UCXTCP
	struct { long length; struct SOCKADDRIN *va;
		int *retadr; long list_end; } sockname;
	sockname.length = sizeof(struct SOCKADDRIN);
	sockname.va = &remote->a;
	sockname.retadr = &remote->length;
	sockname.list_end = 0;
	remote->a.SIN$W_FAMILY = UCX$C_AF_INET;
	remote->a.SIN$W_PORT = 0;	/* any port */
	remote->a.SIN$L_ADDR = 0;

        pthread_mutex_lock ( &tcp_io );
	status = SYS$QIO ( 0, listen_chan, IO$_ACCESS|IO$M_ACCEPT, 
		&iosb, ts_synch_ast, io_cond, 
		0, 0, &sockname, &client_chan, 0, 0 );
	status = ts_synch_io ( status, io_cond, &iosb.status );
#endif
#ifdef MULTINET
        pthread_mutex_lock ( &tcp_io );
	status = SYS$QIO ( 0, client_chan, IO$_ACCEPT, &iosb,
		ts_synch_ast, io_cond, 
		remote, sizeof(socket_info), listen_chan, 0, 0, 0 );
	status = ts_synch_io ( status, io_cond, &iosb.status );
#endif
#ifdef TCPWARE
        pthread_mutex_lock ( &tcp_io );
	status = SYS$QIO ( 0, client_chan, IO$_ACCEPT, &iosb,
		ts_synch_ast, io_cond, 
		remote, sizeof(socket_info), listen_chan, 0, 0, 0 );
	status = ts_synch_io ( status, io_cond, &iosb.status );
#endif
#ifdef CMUTCP
    struct cnx_info {
	unsigned short foreign_size, local_size;		/* name sizes */
	char foreign_host[128];
	unsigned long foreign_port;			/* Only low bytesused*/
	char local_host[128];				/* host names */
	unsigned long local_port;
	unsigned long local_addr, foreign_addr;		/* IP addresses */
    } connect_info;

        pthread_mutex_lock ( &tcp_io );
	status = SYS$QIO ( 0, client_chan, IO$_CREATE, &iosb,
		ts_synch_ast, io_cond,
		CMU_LISTEN_ADDRESS, 0, port_num,
		CMU_LISTEN_FLAGS, 0, 0);
	status = ts_synch_io ( status, io_cond, &iosb.status );
	if ( (status&1) == 0 ) return status;
	/*
	 * Get address info on this connection and build sockaddr structure.
	 */
        pthread_mutex_lock ( &tcp_io );
	status = SYS$QIO ( 0, client_chan, IO$_MODIFY, &iosb,
		ts_synch_ast, io_cond, 
		&connect_info, sizeof(connect_info), 0, 0, 0, 0 );
	status = ts_synch_io ( status, io_cond, &iosb.status );
	if ( (status&1) == 1 ) {
	    remote->a.sin_family = PF_INET;
	    remote->a.sin_port = HTONS(connect_info.foreign_port);
	    remote->a.sin_addr.s_addr = connect_info.foreign_addr;
	} else {
	    /* Assume connection broken and fake the info.  Further
	     * reads should fail, but that will be handled.
	     */
	    status = 1
	    remote->a.sin_family = PF_INET;
	    remote->a.sin_port = 0;
	    remote->a.sin_addr.s_addr = 0;
	}
#endif
    return status;
}
/***************************************************************************/
/* Do interface-specific operations neccessary to set TCP channel for
 * recieving connections on specified port number.
 */
static int create_listen_socket 
	( int port_num,  short chan, pthread_cond_t *cond )
{
    int status, opt_val, i, SYS$QIO();
    struct { short status, count; long ddep; } iosb;
#ifdef TWGTCP
    struct sockaddr_in sock;

    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_SOCKET, &iosb,
	ts_synch_ast, cond,  PF_INET, SOCK_STREAM, 0, 0, 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;

    /* Set socket option on socket to allow quick reuse of address */
    opt_val = 1;
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_SETSOCKOPT, &iosb, ts_synch_ast, cond,
		SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val), 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );

    /* Set socket option to verify connection periodically */
    opt_val = 1;
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_SETSOCKOPT, &iosb, ts_synch_ast, cond,
		SOL_SOCKET, SO_KEEPALIVE, &opt_val, sizeof(opt_val), 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;

    /* Bind socket to port number */
    sock.sin_family = PF_INET;
    sock.sin_port = HTONS(port_num);
    sock.sin_addr.s_addr = local_ip_addr;
    for(i=0;i<sizeof(sock.sin_zero);i++) sock.sin_zero[i] = 0;

    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_BIND, &iosb, ts_synch_ast, cond,
		&sock, sizeof(sock), 0, 0, 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;
    /*
     * Set backlog for connect requests.
     */
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_LISTEN, &iosb,	ts_synch_ast, cond,
		3, 0, 0, 0, 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;
#endif
#ifdef UCXTCP
    int flag;
    struct sockname { long length; int *va; int *retadr; long list_end;
	struct SOCKADDRIN a; };
    struct sockname local;
    struct sockchar { short protocol; char ptype, domain; } sockchar;
    struct  itlst { short length, code;
	  union { struct itlst *lst; int *val; } addr; } olst[3];

    sockchar.protocol = 6; /* UCX$C_TCP; */
    sockchar.ptype =  1; /* UCX$C_STREAM; */
    sockchar.domain = 2; /* UCX$C_AF_INET; */

    local.length = sizeof(struct SOCKADDRIN);
    local.va = (int *) &local.a;
    local.retadr = 0;
    local.list_end = 0;
    local.a.SIN$W_FAMILY = UCX$C_AF_INET;
    local.a.SIN$W_PORT = HTONS(port_num);
    local.a.SIN$L_ADDR = local_ip_addr;
    /*
     * Create socket with parameters we want.
     */
    olst[0].code = UCX$C_SOCKOPT; olst[0].length = 2*sizeof(struct itlst);
    olst[0].addr.lst = &olst[1];
    olst[1].length = 4; olst[1].code = UCX$C_REUSEADDR;
    olst[1].addr.val = &flag; flag = 1;
    olst[2].length = 4; olst[2].code = UCX$C_KEEPALIVE;
    olst[2].addr.val = &flag;

    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_SETMODE, &iosb,
		ts_synch_ast, cond, &sockchar, 0, 0, 0, 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;

    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_SETMODE, &iosb,
		ts_synch_ast, cond, 0, 0, 0, 0, &olst, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;
    /*
     * Bind socket.
     */
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_SETMODE, &iosb,
		ts_synch_ast, cond, 0, 0, &local, 3, 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;
#endif    
#ifdef MULTINET
    struct sockaddr_in sock;

    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_SOCKET, &iosb,
		ts_synch_ast, cond, AF_INET, SOCK_STREAM, 0, 0, 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;

    /* Set socket option on socket to allow quick reuse of address */
    opt_val = 1;
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_SETSOCKOPT, &iosb, ts_synch_ast, cond, 
		SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val), 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );

    /* Set socket option to verify connection periodically */
    opt_val = 1;
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_SETSOCKOPT, &iosb, ts_synch_ast, cond,
		SOL_SOCKET, SO_KEEPALIVE, &opt_val, sizeof(opt_val), 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;

    /* Bind socket to port number */
    sock.sin_family = AF_INET;
    sock.sin_port = HTONS(port_num);
    sock.sin_addr.s_addr = local_ip_addr;
    for(i=0;i<sizeof(sock.sin_zero);i++) sock.sin_zero[i] = 0;

    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_BIND, &iosb, ts_synch_ast, cond,
		&sock, sizeof(sock), 0, 0, 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;
    /*
     * Set backlog for connect requests.
     */
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_LISTEN, &iosb, ts_synch_ast, cond, 
		3, 0, 0, 0, 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;
#endif
#ifdef TCPWARE
    struct sockaddr_in sock;

    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_SOCKET, &iosb,
		ts_synch_ast, cond, AF_INET, SOCK_STREAM, 0, 0, 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;

    /* Set socket option on socket to allow quick reuse of address */
    opt_val = 1;
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_SETSOCKOPT, &iosb, ts_synch_ast, cond, 
		SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val), 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );

    /* Set socket option to verify connection periodically */
    opt_val = 1;
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_SETSOCKOPT, &iosb, ts_synch_ast, cond,
		SOL_SOCKET, SO_KEEPALIVE, &opt_val, sizeof(opt_val), 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;

    /* Bind socket to port number */
    sock.sin_family = AF_INET;
    sock.sin_port = HTONS(port_num);
    sock.sin_addr.s_addr = local_ip_addr;
    for(i=0;i<sizeof(sock.sin_zero);i++) sock.sin_zero[i] = 0;

    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_BIND, &iosb, ts_synch_ast, cond,
		&sock, sizeof(sock), 0, 0, 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;
    /*
     * Set backlog for connect requests.
     */
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, chan, IO$_LISTEN, &iosb, ts_synch_ast, cond, 
		3, 0, 0, 0, 0, 0 );
    status = ts_synch_io ( status, cond, &iosb.status );
    if ( (status&1) == 0 ) return status;
#endif
#ifdef CMUTCP
    status = SYS$DASSGN ( chan );
#endif
    return status;
}
/***************************************************************************/
static int ts_startup_client ( client_ctx ctx )
{
    auto int auto_var;
    int status;
    pthread_setspecific ( client_key, ctx );
    ctx->stack_top = &auto_var;
    status = (*ctx->start)(ctx, ctx->port_num, 
		(unsigned char *) &ctx->remote_address, 
		ctx->index, ctx->clients_remaining );
    return status;
}
/***************************************************************************/
int ts_tcp_stack_used ()
{
    char auto_var;
    client_ctx ctx;

    if ( pthread_getspecific ( client_key, (pthread_addr_t *) &ctx ) )
	return -1;
    if ( !ctx ) return 0;
    return ((int)ctx->stack_top - (int)&auto_var);
}
/***************************************************************************/
int ts_tcp_info ( int *local_port, int *remote_port, 
	unsigned int *remote_address )
{
    int status;
    client_ctx ctx;
    status = pthread_getspecific ( client_key, (pthread_addr_t *) &ctx );
    if ( status != 0 ) return status;
    if ( ctx == (client_ctx) 0 ) return -1;

    *local_port = ctx->port_num;
    *remote_port = ctx->remote_address.TCPSEL(SIN$W_PORT,sin_port,sin_port,
		sin_port, sin_port);
    *remote_port = HTONS(*remote_port);
    *remote_address = ctx->remote_address.TCPSEL(SIN$L_ADDR,sin_addr.s_addr,
		sin_addr.s_addr,sin_addr.s_addr,sin_addr.s_addr);
    return 0;
}
/***************************************************************************/
/* Define routine to return host name for current connection.  The return
 * value is a pointer to a statically allocated area that holds the host name
 * or a formatted representation of the host address.
 */
char *ts_tcp_remote_host ( )
{
    int length, status, i, octet, slot;
    unsigned long remote_address;
    unsigned char *octet_p;
    char *cp;
    client_ctx ctx;
    /*
     * Locate the context block for this thread and validate it.
     */
    status = pthread_getspecific ( client_key, (pthread_addr_t *) &ctx );
    if ( status != 0 ) return (char *) 0;	/* error */
    if ( ctx == (client_ctx) 0 ) return (char *) 0;
    /*
     * Check status of host name and return it if already retrieved.
     */
    if ( ctx->hn_status ) return &ctx->hostname[0];
    /*
     * See if address is in local cache.
     */
    remote_address = (unsigned long) ctx->remote_address.TCPSEL(SIN$L_ADDR,
		sin_addr.s_addr,sin_addr.s_addr,sin_addr.s_addr,sin_addr.s_addr);
    slot = ((remote_address>>24)^((remote_address>>16)&255)) & (CB_SIZE-1);
    pthread_mutex_lock ( &tcp_ctl );		/* Lock while examining cache*/
    if ( (cb[slot].status == 1) && (cb[slot].address == remote_address)) {
	    ctx->hn_status = 1;
	    tu_strcpy ( ctx->hostname, cb[slot].hostname );
    }
    pthread_mutex_unlock ( &tcp_ctl );
    /*
     * Check status again after cache check.
     */
    if ( ctx->hn_status ) return ctx->hostname;

    else {
        /*
         * Host name unknown, do lookup specific to TCP driver in use.  
         * Hostname info is stored in client context block using the 
         * following fields:
         *
         *    ctx->dns_chan	Package specific use (short int), initialized
         *			by port_startup() to -1.
         *    ctx->hn_status	Status of ctx->hostname[] array:
         *			   0 Initial state, hostname invalid.
         *			   1 Known, hostname[] valid.
         *			  -1 Unknown, previous lookup attempt failed.
         *			     (buffer valid, formatted string).
         *
         *    ctx->hostname[]	Host name, zero-terminated string.
         */
#ifdef MULTINET
	struct hostent *hostinfo;
	/*
	 * Assume for now that Multinet library is not re-entrant.
	 */
	pthread_lock_global_np();
        hostinfo = 
	    gethostbysockaddr ( &ctx->remote_address,sizeof(ctx->remote_address));
        if ( hostinfo ) {
	    tu_strnzcpy ( ctx->hostname, hostinfo->h_name ?
		hostinfo->h_name : "<error>", sizeof(ctx->hostname)-1 );
	    ctx->hn_status = 1;
        }
	pthread_unlock_global_np();
#endif
#ifdef TCPWARE
	struct hostent *hostinfo;
	/*
	 * Assume for now that Multinet library is not re-entrant.
	 */
	pthread_lock_global_np();
        hostinfo = 
	    gethostbyaddr ( &ctx->remote_address.sin_addr.s_addr,
		sizeof(ctx->remote_address.sin_addr.s_addr),
		ctx->remote_address.sin_family );
        if ( hostinfo ) {
	    tu_strnzcpy ( ctx->hostname, hostinfo->h_name ?
		hostinfo->h_name : "<error>", sizeof(ctx->hostname)-1 );
	    ctx->hn_status = 1;
        }
	pthread_unlock_global_np();
#endif
#ifdef TWGTCP
	struct hostent *hostinfo;
	/*
	 * Assume for now that Multinet library is not re-entrant.
	 */
	pthread_lock_global_np();
        hostinfo = 
	    gethostbyaddr ( &ctx->remote_address.sin_addr.s_addr, 
		sizeof(ctx->remote_address.sin_addr.s_addr),
		ctx->remote_address.sin_family );
        if ( hostinfo ) {
	    tu_strnzcpy ( ctx->hostname, hostinfo->h_name ?
		hostinfo->h_name : "<error>", sizeof(ctx->hostname)-1 );
	    ctx->hn_status = 1;
        }
	pthread_unlock_global_np();
#endif
#ifdef UCXTCP
	struct {
	    long length; unsigned char *va;		/* descriptor */
	    unsigned char func, subfunc, dum1, dum2;
	} p1, p2, p4;
	int SYS$QIO();
        /*
         * setup arguments for ACPCONTROL function.
         */
	length = 0;
	p1.length = 4; p1.va = &p1.func;
	p1.func = 2;		/* gethostbyaddr */
	p1.subfunc = 2;		/* trans */
	p2.length = sizeof(ctx->remote_address.SIN$L_ADDR);
	p2.va = (unsigned char *) &ctx->remote_address.SIN$L_ADDR;
	p4.length = sizeof(ctx->hostname)-1;
	p4.va = (unsigned char *) ctx->hostname;
	ctx->dns_chan = ctx->chan;	

	pthread_mutex_lock ( &tcp_io );
	status = SYS$QIO ( 0, ctx->dns_chan, IO$_ACPCONTROL, &ctx->iosb,
	    ts_synch_ast, &ctx->io_done, &p1, &p2, &length, &p4, 0, 0 );
	status = ts_synch_io ( status, &ctx->io_done, &ctx->iosb[0] );
	if ( (status&1) == 1 ) {
	    /*
	     * Lookup successful, terminate string and change status.
	     */
	    ctx->hn_status = 1;
	    ctx->hostname[length] = '\0';
	}
#endif
#ifdef CMUTCP
	struct { long length; char hostname[256]; } host_buf;
	int SYS$QIO();

	ctx->dns_chan = ctx->chan;
	pthread_mutex_lock ( &tcp_io );
	status = SYS$QIO ( 0, ctx->dns_chan, IO$_SKIPFILE, &ctx->iosb,
	    ts_synch_ast, &ctx->io_done, 
	    &host_buf, sizeof(host_buf), 2, remote_address, 0, 0, 0 );
	status = ts_synch_io ( status, &ctx->io_done, &ctx->iosb[0] );
	if ( (status&1) == 1 ) {
	    /*
	     * Lookup successful, copy result and change status.
	     */
	    ctx->hn_status = 1;
	    if ( host_buf.length > (sizeof(ctx->hostname)-1) )
		host_buf.length = sizeof(ctx->hostname)-1;
	    tu_strnzcpy ( ctx->hostname, host_buf.hostname, host_buf.length );
	}
#endif
    }
    /*
     * If name lookup worked, place in cache.
     */
    if ( ctx->hn_status == 1 ) {
	pthread_mutex_lock ( &tcp_ctl );
	cb[slot].address = remote_address;
	cb[slot].status = 1;
	tu_strcpy ( cb[slot].hostname, ctx->hostname );
	pthread_mutex_unlock ( &tcp_ctl );

    } else {
        /*
         * Unable to determine host name, format it as n.n.n.n, using threadsafe
         * formatting routine (tu_strint).
         */
        ctx->hn_status = -1;
        octet_p = (unsigned char *) &remote_address;
        cp = ctx->hostname;
        for ( i = 0; i < 4; i++ ) {
	    octet = *octet_p++;		/* Convert to int */
	    tu_strint ( octet, cp ); while ( *cp ) cp++;
	    if ( i < 3 ) *cp++ = '.';	/* Delimit each label */
        }
    }
    return ctx->hostname;
}
/***************************************************************************/

static void client_rundown ( client_ctx ctx )
{
    int status, SYS$DASSGN();
    client_ctx t;
    port_ctx parent;
    if ( !ctx ) return;
    /* printf("running down tcp client, index=%d\n", ctx->index ); */
    ctx->status = 2;	/* flag as deleted */
    pthread_mutex_lock ( &tcp_ctl );
    for ( parent = port_list.flink; parent != &port_list; parent =
		parent->flink) {
	if ( parent->port_num == ctx->port_num ) {
	    /* Decrement counts */
	    --parent->client_count;
	    t = (client_ctx) ctx->flink;
	    t->blink = ctx->blink;
	    t = (client_ctx) ctx->blink; t->flink = ctx->flink;
	    break;
	}
    }
    status = SYS$DASSGN ( ctx->chan );
    pthread_cond_destroy ( &ctx->io_done );	/* cleanup condition */
    pthread_detach ( &ctx->thread );
    if ( parent == &port_list ) {
	PUTLOG ( 0, "Client rundown on deleted port (!SL)!/", 
		ctx->port_num );
    } else {
	/* Place on free list, signal anybody waiting for client. */
	ctx->flink = (port_ctx) parent->free_clients;
	parent->free_clients = ctx;
	if ( parent->connect_pending ) {
	    PUTLOG ( 0, "Signalling client available: !%T!/", 0 );
	    pthread_cond_signal ( &parent->client_available );
	}
    }
    pthread_mutex_unlock ( &tcp_ctl );
}

static void port_rundown (port_ctx ctx )
{
    port_ctx t;
    client_ctx client;
    int count;

    LOCK_C_RTL
    printf("running down tcp listen port, ctx address: %d\n", ctx );
    UNLOCK_C_RTL
    if ( !ctx ) return;
    if ( ctx->status ) {
	pthread_mutex_lock ( &port_startup );
	startup_status = ctx->status;
        ctx->status = 0;
	pthread_mutex_unlock ( &port_startup );
	pthread_cond_signal ( &port_startup_done );
    }

    /* Rundown clients */
    pthread_mutex_lock ( &tcp_ctl );
    for ( client = (client_ctx) ctx->client_list.flink;
	   client != &ctx->client_list; client = (client_ctx) client->flink ) {
	pthread_cancel ( client->thread );
    }
    pthread_mutex_unlock ( &tcp_ctl );
    count = ctx->client_count;

    pthread_cond_destroy ( &ctx->client_available );
    pthread_mutex_lock ( &tcp_ctl );
    t = ctx->flink; t->blink = ctx->blink;
    t = ctx->blink; t->flink = ctx->flink;
    pthread_mutex_unlock ( &tcp_ctl );

#ifndef CMUTCP
    SYS$DASSGN ( ctx->chan );
#endif
    pthread_detach ( &ctx->thread );

    LOCK_C_RTL
    for ( client = ctx->free_clients; client; client = ctx->free_clients ) {
	ctx->free_clients = (client_ctx) client->flink;
	free ( client );
    }
    free ( ctx );
    UNLOCK_C_RTL

}
