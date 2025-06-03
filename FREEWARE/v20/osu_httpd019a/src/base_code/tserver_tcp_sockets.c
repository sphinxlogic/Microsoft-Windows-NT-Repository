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
 * Revised: 7-APR-1995		Work around problem with management host.
 * Revised: 19-APR-1995		Add missing semicolon (CMUTCP variant).
 * Revised: 10-MAY-1995		Cleanup for BSDTCP option
 * Revsied: 24-MAY-1995		Close client-chan for restarts to work.
 * Revised: 27-MAY-1995		Split into _sockets version to make code
 *				more accessible (eliminates options).
 * Revised:  3-AUG-1995		Use common client pools for listen ports.
 */
#include "pthread_np.h"
#include "tutil.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef VMS
#include <unixio.h>
#include <types.h>
#include <in.h>
#include <socket.h>
#else   /* OSF */
#include <sys/uio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif
#include <netdb.h>
#include <errno.h>
#include <signal.h>

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
    int chan, dns_chan;
    unsigned long local_ip_addr;
    int clients_remaining;

    struct sockaddr_in remote_address;     /* Remote address */
    int hn_status;		/* Status of hostname item: 0, 1, -1 */
    char hostname[256];		/* Remote host name */
};

struct client_pool {			/* Manage pool of clients */
    struct client_pool *flink;
    pthread_attr_t *client_attr;	/* Attributes for created clients */
    pthread_cond_t client_available;	/* signals freelist is non-empty */
    int ref_count;			/* Number of ports sharing pool */
    int connect_pending;		/* True if thread waiting */
    int client_count, client_limit;
    client_ctx free_clients;
};

struct port_context {
    struct port_context *flink, *blink;
    int port_num;
    int status;
    pthread_t thread;
    unsigned long local_ip_addr;
    int chan;
    int (*start)( client_ctx ctx, 
	  short port, unsigned char *rem_addr,
	  int ndx, int length);	/* Application supplied startup routine */
    int client_count;
    client_ctx_struct client_list;
    struct client_pool *pool;
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
static int local_addr_count;		/* Number of defined addresses */
static int local_addr_alloc;		/* allocated size of list. */
static unsigned long *local_addr_list;	/* local listen address, stored in 
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
static int create_listen_socket(int, unsigned long, int );
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
    if ( local_addr_count == 1 && local_addr_list[0] == 0 )
	local_addr_list[0] = *address;
    else {
	if ( local_addr_count >= local_addr_alloc ) {
	    local_addr_alloc = local_addr_alloc + 32;
	    LOCK_C_RTL
	    local_addr_list = realloc ( local_addr_list,
		sizeof(unsigned long) * local_addr_alloc );
	    UNLOCK_C_RTL
	}
	local_addr_list[local_addr_count++] = *address;
    }
    pthread_mutex_unlock ( &tcp_ctl );
    PUTLOG ( 5, "Address !AZ encodes as !XL!/", addr, *address );
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
    /*
     * If we have an explicit listen address (multi-homed host),
     * it is impossible to get messages from the loopback address,
     * so use the listen address instead for the management host.
     */
    if ( local_addr_list[0] && (ts_manage_host == 0x0100007F) ) 
	ts_manage_host = local_addr_list[0];
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
    int ts_startup_port(), i, stacksize, status, pthread_create();
    port_ctx ctx, tmp;
    struct client_pool *pool;
    pthread_t listen_thread;
    pthread_attr_t port_attr;
    static int marker = 0;
    /*
     * Initialize globals for module, only once though.
     */
    pthread_once ( &ts_mutex_setup, ts_init_mutexes );
    /*
     * Find or create client pool structure for managing contexts.
     */
    if ( client_limit > 0 ) {
	/*
	 * Allocate new pool.
	 */
        pool = (struct client_pool *) malloc ( sizeof(struct client_pool) );
	pool->client_attr = client_attr;
	pool->ref_count = 0;
        pool->connect_pending = 0;
	pool->client_count = 0;
        pool->client_limit = client_limit;
        pthread_cond_init (&pool->client_available, pthread_condattr_default);
	/*
         * Pre-allocate client_limit number client contexts for the client threads.
         */
        pool->free_clients = (client_ctx) 0;
        LOCK_C_RTL
        for ( i = 0; i < pool->client_limit; i++ ) {
	    client_ctx new;
	    new = (client_ctx) malloc ( sizeof(client_ctx_struct) );
	    new->index = marker+(pool->client_limit-i);
	    new->dns_chan = -1;
            new->hn_status = 0;
	    new->flink = (port_ctx) pool->free_clients;
	    pool->free_clients = new;
        }
        marker += pool->client_limit;	/* base for thread index numbers */
        UNLOCK_C_RTL
    } else {
	/*
	 * Lock port list and scan ports for pool with matching client_attr 
	 * address.
	 */
	pthread_mutex_lock ( &tcp_ctl );
	for ( tmp = port_list.flink; tmp != &port_list; tmp = tmp->flink ) {
	    if ( tmp->pool->client_attr == client_attr ) {
	        pool = tmp->pool;
	        break;
	    }
	}
        pthread_mutex_unlock ( &tcp_ctl );
	if ( tmp == &port_list ) return 20;
    }
    /*
     * Create thread attributes object for the listener thread(s).  Use FIFO
     * scheduling which will give it a higher priority than the client
     * threads which have round robin scheduling.
     */                                                              
    status = pthread_attr_create ( &port_attr );
    ERROR_FAIL(status,"Port thread attribute create failure" )
    status = pthread_attr_setinheritsched ( &port_attr, PTHREAD_DEFAULT_SCHED );
#ifndef REALTIME_SCHED
    /* Can't adjust priorities */
#ifdef VMS
    status = pthread_attr_setsched ( &port_attr, SCHED_BG_NP );
    status = pthread_attr_setprio ( &port_attr, (PRI_BG_MAX_NP) );
#endif
    status = pthread_attr_setstacksize ( &port_attr, PORT_THREAD_STACK*4 );
#else
    /* Make port listener thread higher priority */
    status = pthread_attr_setsched ( &port_attr, SCHED_FIFO );
    status = pthread_attr_setprio ( &port_attr, (PRI_FIFO_MIN+PRI_FIFO_MAX)/2 );
    status = pthread_attr_setstacksize ( &port_attr, PORT_THREAD_STACK );
#endif
    /*
     * Create listener thread for each local address in list.
     */
    for ( i = 0; i < local_addr_count; i++ ) {
        /*
         * Allocate port control block that becomes the thread-specific context
         * data for the new thread.
         */
        LOCK_C_RTL
        ctx = (port_ctx) malloc ( sizeof(port_ctx_struct) );
        UNLOCK_C_RTL
        /*
         * Initialize pertinent fields in control block.
         */
        ctx->status = 0;
        ctx->start = start;
        ctx->client_count = 0;
        ctx->pool = pool;
        /*
         * Link the control block into the global list and connect pool, use 
         * mutex to synchronize update of pointers.
         */
        pthread_mutex_lock ( &tcp_ctl );
        ctx->port_num = port_num;
        ctx->local_ip_addr = local_addr_list[i];
        PUTLOG(3,"declaring local address: !XL!/", ctx->local_ip_addr );
        ctx->flink = &port_list;
        ctx->blink = port_list.blink;
        port_list.blink->flink = ctx;
        port_list.blink = ctx;
        pool->ref_count++;
        pthread_mutex_unlock   ( &tcp_ctl );
        /* 
         * Create thread and block until it finishes initializing so we can
         * check it's status.
         */
        status = pthread_mutex_lock ( &port_startup );
        ERROR_FAIL(status,"Port startup lock failure" )
        startup_status = 0;

        status = pthread_create ( (i == 0 ) ? control_thread : &listen_thread, 
		port_attr, ts_startup_port, ctx);
        ERROR_FAIL(status,"Port thread create failure" )

        while ( startup_status == 0 ) {
            status = pthread_cond_wait ( &port_startup_done, &port_startup );
            ERROR_FAIL(status,"Port startup wait failure" )
        }
        pthread_mutex_unlock ( &port_startup );
    }

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
    int status;
    /*
     * The send function can generated a SIGPIPE signal, therefore setup
     * exception handling block around the write.
     */
    TRY {
        status = send ( ctx->chan, buffer, length, 0 );
        if ( status < 0 ) status = errno * 8;
        else status = 1;
    } CATCH (exc_SIGPIPE_e) {
	status = 8428;		/* SS$_LINKDISCON */
	PUTLOG(1,"TCP-!SL/!SL Connection to client broke, write failed.!/",
		ctx->port_num, ctx->index );
    } CATCH_ALL {
	RERAISE;
    } ENDTRY
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
    int status, size;
    size = recv ( ctx->chan, buffer, maxlen, 0 );
    if ( size > 0 ) { *length = size; status = 1; }
    else {
	*length = 0;
	status = (size == 0) ? 2160 : errno*8;
    }
    return status;
}
/**************************************************************************/
/* Perform synchronous disconnect of socket.  Do not deassign channel.
 */
int ts_tcp_close
    ( client_ctx ctx )		/* Context passed to port start() function */
{
    int status;
    close ( ctx->chan );
    status = 1;
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
    local_addr_count = 1;
    local_addr_alloc = 16;
    LOCK_C_RTL
    local_addr_list = (unsigned long *) 
	malloc ( sizeof(unsigned long)*local_addr_alloc );
    UNLOCK_C_RTL
    local_addr_list[0] = 0;
    ts_manage_port = -1;
    ts_manage_host = 0;
}
/***********************************************************************/
/*
 * Top level of thread for listening on ports  Upon entry, the calling
 * thread will be waiting on the port_startup_done condition.
 */
int ts_startup_port ( port_ctx ctx )
{
    int status, stacksize, i;
    int client_chan;
    socket_info remote;
    pthread_cond_t io_done;
    client_ctx new_client;
    struct client_pool *pool;
    /*
     * Create context key for this thread and init to our context block address.
     * The port rundown routine can then cleanup.
     */
    pthread_setspecific ( port_key, ctx );
    ctx->thread = pthread_self();
    ctx->client_list.flink = (port_ctx) &ctx->client_list;
    ctx->client_list.blink = (port_ctx) &ctx->client_list;
    ctx->client_list.index = 0;
    /*
     * Assign control channel for listens.
     */
    ctx->chan = 0;
    ctx->chan = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( ctx->chan < 1 ) { ctx->status = 20; return 20; }
    else {
	/*
	 * Bind address that we will listen on.
	 */
	ctx->status = create_listen_socket 
		(  ctx->port_num, ctx->local_ip_addr, ctx->chan);
    }
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
	remote.length = sizeof(remote.a);
	client_chan = accept ( ctx->chan, 
		(struct sockaddr *) &remote.a, &remote.length );
	if ( client_chan < 0 ) { status = errno; break; }
printf("Connect accepted\n");
	/*
	 * See if connect is from privileged management port.  Note that 
	 * port number is in network data order.  Since ts_manage_port is
	 * a shared variable, get mutex before examinging it.
	 */
	current_port = remote.a.sin_port;
	pthread_mutex_lock ( &tcp_ctl );
	if ( current_port == ts_manage_port ) {
	    /*
	     * Only accept from proper host.
	     */
	    unsigned int current_address;
	    current_address = remote.a.sin_addr.s_addr;
	    if ( current_address == ts_manage_host ) {
		/*
		 * If port_manage returns a non-zero value, exit loop with that
		 * status.  Close client connection in all cases afterward.
		 */
		pthread_mutex_unlock ( &tcp_ctl );
		status = port_manage ( ctx, client_chan, &remote );
	        close(client_chan);
		if ( status ) break;
		continue;
	    }
	}
	/*
	 * Allocate client context block from free list.  Grab mutex while
	 * while playing with module-wide data structures.
	 */
	pool = ctx->pool;
	new_client = pool->free_clients;
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
		pool->connect_pending = 1;
	        pthread_cond_timedwait 
		    ( &pool->client_available, &tcp_ctl, &abstime );
		pool->connect_pending = 0;
		new_client = pool->free_clients;
	    } else PUTLOG ( 0, "Time computation problem!/" );
	}

	if ( new_client ) {
	    /*
	     * We got block, initialize it.
	     */
	    pool->free_clients = (client_ctx) new_client->flink;
	    pool->client_count++;
	    ctx->client_count++;
	    new_client->flink = (port_ctx) &ctx->client_list;
	    new_client->blink = ctx->client_list.blink;
	    ctx->client_list.blink = (port_ctx) new_client;
	    new_client->blink->flink = (port_ctx) new_client;
	    new_client->status = 0;
	    new_client->hn_status = 0;		/* host name unknown */
	    new_client->port_num = ctx->port_num;
	    new_client->local_ip_addr = ctx->local_ip_addr;
	    new_client->clients_remaining = 
			pool->client_limit - pool->client_count;
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

    	    status = pthread_create ( &new_client->thread, *(pool->client_attr),
		ts_startup_client, new_client );
	    if ( status == -1 ) perror ( "error creating client thread" );
	} else {
	    pthread_mutex_lock ( &tcp_ctl );
	    PUTLOG ( 0, "Connect reject, no free client contexts!/" );
	    pthread_mutex_unlock ( &tcp_ctl );
	    close(client_chan);
	}
    }
    /* Cleanup resources and return final status */
    pthread_cond_destroy ( &io_done );
    close(ctx->chan);
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
    struct client_pool *pool;
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
	 * Wait for existing clients in pool to rundown.
	 */
	pthread_mutex_lock ( &tcp_ctl );
	PUTLOG ( 1, "shutting down TCP listenport !SL!/", ctx->port_num );
	pool = ctx->pool;
	count = pool->client_count;		/* initial active */
	while ( pool->client_count > 0 ) {
	    pool->connect_pending = 1;
	    if ( 0 > pthread_cond_timedwait 
		    ( &pool->client_available, &tcp_ctl, &abstime ) ) break;
	}
	pool->connect_pending = 0;
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
/* Do interface-specific operations neccessary to set TCP channel for
 * recieving connections on specified port number.
 */
static int create_listen_socket ( int port_num, 
	unsigned long local_ip_addr, int chan )
{
    int status, opt_val, opt_size, i;
    struct { short status, count; long ddep; } iosb;
    struct sockaddr_in sock;
    /*
     * Set socket options: REUSEADDR, KEEPALIVE
     */
    opt_val = 1;
    opt_size = sizeof(int);
    status = setsockopt ( chan, SOL_SOCKET, SO_KEEPALIVE, 
		(char *) &opt_val, opt_size );
    if (status == -1) PUTLOG(0,"setsockopt status (keepalive): !SL!/", status );
    status = setsockopt ( chan, SOL_SOCKET, SO_REUSEADDR, 
		(char *) &opt_val, opt_size );
    if (status == -1) PUTLOG(0,"setsockopt status (reuseaddr): !SL!/", status );
    /*
     * Bind the address.
     */
    sock.sin_family = AF_INET;
    sock.sin_port = HTONS(port_num);
    sock.sin_addr.s_addr = local_ip_addr;
    for ( i = 0; i < 8; i++ ) sock.sin_zero[i] = '\0';
    status = bind ( chan, (struct sockaddr *) &sock, sizeof(sock) );
    if ( status < 0 ) return 20;

    status = listen ( chan, 5 );
    return (status < 0 ) ? 20 : 1;
}
/***************************************************************************/
static int ts_startup_client ( client_ctx ctx )
{
    auto int auto_var;
    int status;
    pthread_setspecific ( client_key, ctx );
    ctx->stack_top = &auto_var;
    TRY {
    status = (*ctx->start)(ctx, ctx->port_num, 
		(unsigned char *) &ctx->remote_address, 
		ctx->index, ctx->clients_remaining );
    }
    CATCH_ALL {
	PUTLOG(0,"TCP-!SL/!SL, thread terminating due to exception!/", 
		ctx->port_num, ctx->index );
	exc_report ( THIS_CATCH );
    }
    ENDTRY
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
    if ( remote_port ) {
        *remote_port = ctx->remote_address.sin_port;
        *remote_port = HTONS(*remote_port);
        *remote_address = ctx->remote_address.sin_addr.s_addr;
    } else {
	/*
	 * Hack for multi-homed operation, return local ip address.
	 */
 	*remote_address = (unsigned int) ctx->local_ip_addr;
    }
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
    remote_address = (unsigned long) ctx->remote_address.sin_addr.s_addr;
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
	struct hostent *hostinfo;
	/*
	 * Assume for now that Multinet library is not re-entrant.
	 */
	pthread_lock_global_np();
        hostinfo = 
	    gethostbyaddr ( (char *) &ctx->remote_address.sin_addr.s_addr,
		sizeof(ctx->remote_address.sin_addr.s_addr),
		ctx->remote_address.sin_family );
        if ( hostinfo ) {
	    tu_strnzcpy ( ctx->hostname, hostinfo->h_name ?
		hostinfo->h_name : "<error>", sizeof(ctx->hostname)-1 );
	    ctx->hn_status = 1;
        }
	pthread_unlock_global_np();
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
    int status;
    client_ctx t;
    port_ctx parent;
    struct client_pool *pool;
    if ( !ctx ) return;
    /* printf("running down tcp client, index=%d\n", ctx->index ); */
    ctx->status = 2;	/* flag as deleted */
    pthread_mutex_lock ( &tcp_ctl );
    for ( parent = port_list.flink; parent != &port_list; parent =
		parent->flink) {
	if ( (parent->port_num == ctx->port_num) && 
		(parent->local_ip_addr == ctx->local_ip_addr) ) {
	    /* Decrement counts */
	    --parent->client_count;
	    --parent->pool->client_count;
	    t = (client_ctx) ctx->flink;
	    t->blink = ctx->blink;
	    t = (client_ctx) ctx->blink; t->flink = ctx->flink;
	    break;
	}
    }
    close ( ctx->chan ); status = 1;
    pthread_cond_destroy ( &ctx->io_done );	/* cleanup condition */
    pthread_detach ( &ctx->thread );
    if ( parent == &port_list ) {
	PUTLOG ( 0, "Client rundown on deleted port (!SL)!/", 
		ctx->port_num );
    } else {
	/* Place on free list, signal anybody waiting for client. */
	pool = parent->pool;
	ctx->flink = (port_ctx) pool->free_clients;
	pool->free_clients = ctx;
	if ( pool->connect_pending ) {
	    PUTLOG ( 0, "Signalling client available: !%T!/", 0 );
	    pthread_cond_signal ( &pool->client_available );
	}
    }
    pthread_mutex_unlock ( &tcp_ctl );
}

static void port_rundown (port_ctx ctx )
{
    port_ctx t;
    client_ctx client;
    struct client_pool *pool;
    int count, ref_count;

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

    pthread_mutex_lock ( &tcp_ctl );
    t = ctx->flink; t->blink = ctx->blink;
    t = ctx->blink; t->flink = ctx->flink;
    pool = ctx->pool;
    --pool->ref_count;
    ref_count == pool->ref_count;
    pthread_mutex_unlock ( &tcp_ctl );

    pthread_detach ( &ctx->thread );

   if ( ref_count == 0 ) {
      /*
       * Nobody left to use pool, deallocate it.
       */
      LOCK_C_RTL
      for ( client=pool->free_clients; client; client = pool->free_clients ) {
	pool->free_clients = (client_ctx) client->flink;
	free ( client );
      }
      pthread_cond_destroy ( &pool->client_available );
      free ( pool );
      UNLOCK_C_RTL
    }
    LOCK_C_RTL
    free ( ctx );
    UNLOCK_C_RTL

}
