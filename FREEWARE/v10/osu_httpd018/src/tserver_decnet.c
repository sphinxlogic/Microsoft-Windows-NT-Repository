/*
 * This pthread module creates a thread that listens on a specified DECNet
 * object name or number for incoming connections.  Each connection received 
 * creates a new thread to handle the session.  You may call
 * ts_declare_decnet_object multiple times to listen for more that one object
 * name or number.   On thread exit, the connection is
 * closed.
 *
 * If a connection is received and no client contexts are available, the
 * listener thread will wait for up to 30 seconds for a context
 * to free up.
 *
 * int ts_declare_decnet_object ( task_name, client_limit, attr, thread, start );
 *	int start ( ctx, port, remote_address );
 * int ts_decnet_write ( ctx, buffer, length );
 * int ts_decnet_read ( ctx, buffer, maxlen, *length );
 * int ts_decnet_close ( ctx );
 * int ts_decnet_info ( char *taskname_num, char *rem_node, char *rem_user );
 * int ts_decnet_set_access ( char *log_name );
 *
 * Revised: 1-DEC-1994		! added netserver (single shot) support.
 * Revsied: 18-DEC-1994		Added logname-based access control.
 */
#include "pthread_np.h"
#include "tutil.h"

#include <stdlib.h>
#include <stdio.h>
#include <descrip.h>
#include <iodef.h>
#include <lnmdef.h>
#include <nfbdef.h>
#include <msgdef.h>
int SYS$DASSGN(), SYS$TRNLNM();
static $DESCRIPTOR(decnet_device,"_NET:");
static $DESCRIPTOR(decnet_sysnet,"SYS$NET");
static $DESCRIPTOR ( lnm_file_dev, "LNM$FILE_DEV");

#define ERROR_FAIL(s,text) if (s == -1) perror(text);
#define PORT_THREAD_STACK 4000
#define ts_decnet_stack_used ts_tcp_stack_used

struct ncbdef { long length; char *va; char data[256]; };

struct client_context {
    struct port_context *flink, *blink;
    int index, obj_num;
    int unit;
    struct port_context *parent;
    int status;
    pthread_t thread;
    int *stack_top;		/* Top address (nearly) of thread's stack */
    pthread_cond_t io_done;
    int (*start)();		/* Application supplied startup routine */
    short chan;			/* Network channel */
    short fill;
    int clients_remaining;
    short iosb[4];

    struct ncbdef ncb;
};
typedef struct client_context client_ctx_struct, *client_ctx;

struct mailbox_message { short type, unit;
    unsigned char name_len;
    char info[295];
};

struct port_context {
    struct port_context *flink, *blink;
    int obj_num, unit;
    int status;
    pthread_t thread;
    char *object_name;			/* Only valid during port startup */
    int connect_pending;		/* Connection is pending */
    pthread_cond_t client_available;	/* signals freelist is non-empty */
    pthread_attr_t *client_attr;	/* Attributes for created clients */
    short chan, mbx_chan;
    int (*start)();
    int client_count, client_limit;
    client_ctx_struct client_list;
    client_ctx free_clients;
};
typedef struct port_context port_ctx_struct, *port_ctx;
#define ts_client_ctx client_ctx
#include "tserver_decnet.h"

struct access_entry { int length; char node_user[260]; };
static int trusted_host_count;		/* If zero, do no access checks */
static struct access_entry *trusted_host_list;
static port_ctx_struct port_list;
static int ts_startup_client ( client_ctx ctx );
port_ctx tserver_decnet_port_list;

static int ts_logging = 0;
static int (*ts_putlog)(int, char *, ... );	/* upcall for logging */
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
static pthread_once_t ts_mutex_setup = pthread_once_init;
static int startup_status;		/* Predicate */

static pthread_key_t port_key;		/* Private data for port listener */
static pthread_key_t client_key;	/* Private data for TCP client */
static void ts_init_mutexes();		/* forward reference */
static int connect_accept();
static int ts_synch_ast(), ts_synch_io();
/*************************************************************************/
/* Setup list of trusted clients by translating specified logical name.
 * Logical name is multivalue logical containing 1 nodename/user per
 * equivalence name.  Must be called prior to declaring any ports.
 */
int ts_set_access ( char *log_name )
{
    int length, count, i, j, k, status;
    struct { short len, code; char *buffer; int *retlen, term; } item;
    struct { short len, code; char *buffer; int *retlen; } iteml[3];
    struct access_entry *host_list;
    char equiv_name[256];
    struct dsc$descriptor log_name_dx;
    /*
     * Make sure globals are inited.
     */
    pthread_once ( &ts_mutex_setup, ts_init_mutexes );
    /*
     * First see if logical name is valid and get max_index.
     */
    item.len = sizeof(count);
    item.code = LNM$_MAX_INDEX;
    item.buffer = (char *) &count;
    item.retlen = 0;
    length = item.term = count = 0;
    log_name_dx.dsc$b_dtype = DSC$K_DTYPE_T;		/* text data */
    log_name_dx.dsc$b_class = DSC$K_CLASS_S;		/* fixed (Static) */
    log_name_dx.dsc$w_length = tu_strlen ( log_name );
    log_name_dx.dsc$a_pointer = log_name;
    
    status = SYS$TRNLNM ( 0, &lnm_file_dev, &log_name_dx, 0, &item );
    if ( (status&1) == 0 ) return status;
    count++;		/* index starts at zero */
    /*
     * Allocate trusted host list, one entry for each equiv. name.
     */
    LOCK_C_RTL
    if ( count > 0 ) host_list = (struct access_entry *) malloc 
		( sizeof(struct access_entry) * count );
    if ( !host_list ) return 0;
    UNLOCK_C_RTL
    /*
     * Populate list.
     */
    iteml[0].len = sizeof(i);
    iteml[0].code = LNM$_INDEX;
    iteml[0].buffer = (char *) &i;
    iteml[0].retlen = (int *) 0;
    iteml[1].len = sizeof(equiv_name)-1;
    iteml[1].code = LNM$_STRING;
    iteml[1].buffer = equiv_name;
    iteml[1].retlen = &length;
    iteml[2].len = iteml[2].code = 0;	/* terminate list */
    for ( i = 0; i < count; i++ ) {
	/* Translate name */
        status = SYS$TRNLNM ( 0, &lnm_file_dev, &log_name_dx, 0, &iteml );
        if ( (status&1) == 0 ) return status;
	/*
 	 * Construct node_user from equivialence name.  Node user
	 * is in same format as ncb data for efficient checks.
	 *  node::user -> node::"0=user/
	 */
	for ( j = k = 0; j < length; j++ ) {
	    host_list[i].node_user[k++] = equiv_name[j];
	    if ( (j > 0) && (equiv_name[j] == ':') ) {
		if ( ((j+1) == k) && (equiv_name[j-1] == ':') ) {
		    /* 
		     * Found end of nodename, insert '"0='
		     */
		    host_list[i].node_user[k++] = '"';
		    host_list[i].node_user[k++] = '0';
		    host_list[i].node_user[k++] = '=';
		}
	    }
        }
	host_list[i].node_user[k++] = '/';
	host_list[i].length = k;
    }
    /*
     * Grab mutex and update global list.
     */
    pthread_mutex_lock ( &tcp_ctl );
    trusted_host_count = count;
    trusted_host_list = host_list;
    pthread_mutex_unlock ( &tcp_ctl );
}
/*************************************************************************/
/* Set flag and putlog callback routine address.
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
/* Create thread that listens on a particular TCP/IP port and starts
 * new thread for each accepted connection.  A caller supplied start routine
 * is called by the created thread to process the session.
 *
 * This routine can be called multiple times within an application to listen
 * on different ports.  Each port gets it's own client limit.
 *
 * User start (session) routine:
 *	int start ( void *ctx, ncb );
 */
int ts_declare_decnet_object ( 
    char *object_name_num,	/* Object name (or number) */
    int client_limit, 		/* Limit on concurrent client threads allowed */
    pthread_attr_t *client_attr, /* Thread attributes for client threads */
    pthread_t *control_thread, 	/* Thread that listens for connects */
    int start() )		/* Start routine for new clients. */
{
    int ts_startup_object(), stacksize, status, pthread_create();
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
    ctx->flink = &port_list;
    ctx->blink = port_list.blink;
    port_list.blink->flink = ctx;
    port_list.blink = ctx;
    pthread_mutex_unlock   ( &tcp_ctl );
    /*
     * Zero pertinent fields in control block.
     */
    ctx->status = 0;
    ctx->object_name = object_name_num;
    ctx->start = start;
    ctx->client_attr = client_attr;
    ctx->client_count = 0;
    ctx->client_limit = client_limit;
    /* 
     * Create thread atributes for the listener thread (use FIFO scheduling
     * which has a higher priority than the Round Robin client threads.
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

    status = pthread_create (control_thread, port_attr, ts_startup_object, ctx);
    ERROR_FAIL(status,"Port thread create failure" )

    while ( startup_status == 0 ) {
        status = pthread_cond_wait ( &port_startup_done, &port_startup );
        ERROR_FAIL(status,"Port startup wait failure" )
    }
    status = startup_status;
    pthread_mutex_unlock ( &port_startup );

    return status;
}
/**************************************************************************/
/* Write data to thread's DECnet connection.
 */
int ts_decnet_write 
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
	 * Only write up to 2 K at a time.
	 */
	seg = remaining > 2048 ? 2048 : remaining;
        pthread_mutex_lock ( &tcp_io );
        status = SYS$QIO ( 0, ctx->chan, IO$_WRITEVBLK, &ctx->iosb,
	    ts_synch_ast, &ctx->io_done, buffer, seg, 0, 0,
	    0, 0 );
	status = ts_synch_io ( status, &ctx->io_done, &ctx->iosb[0] );
        if ( (status&1) == 0 ) break;

	seg = ctx->iosb[1];	/* length actually sent */
	buffer = &buffer[seg];   /* Point to next unsent char */
    }
    return status;
}
/**************************************************************************/
/* Read data from thread's DECnet connection.
 */
int ts_decnet_read
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
	*length = ctx->iosb[1];
    } else *length = 0;
    return status;
}
/**************************************************************************/
/* Perform synchronous disconnect of socket.  Do not deassign channel.
 */
int ts_decnet_close
    ( client_ctx ctx )		/* Context passed to port start() function */
{
    int SYS$QIO(), status, ts_synch_ast(), ts_synch_io();
    /*
     * Make $QIO call and block until AST signals us.
     */
    status = 1;
    pthread_mutex_lock ( &tcp_io );
    status = SYS$QIO ( 0, ctx->chan, IO$_DEACCESS|IO$M_SYNCH, &ctx->iosb,
	    ts_synch_ast, &ctx->io_done, 0, 0, 0, 0, 0, 0 );
    status = ts_synch_io ( status, &ctx->io_done, &ctx->iosb[0] );
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
    tserver_decnet_port_list = &port_list;	/* set global pointer */
    trusted_host_count = 0;
    pthread_mutex_lock ( &tcp_ctl );
    port_list.flink = &port_list; port_list.blink = &port_list;
    port_list.unit = 0;				/* flag list header */
    pthread_mutex_unlock ( &tcp_ctl );
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
int ts_startup_object ( port_ctx ctx )
{
    int status, stacksize, i, permanent, SYS$ASSIGN(), SYS$QIO();
    int client_chan, LIB$ASN_WTH_MBX(), maxmsg, bufquo;
    struct iosb_t  { short status, count, d1, d2; } iosb;
    struct mailbox_message msg;
    pthread_cond_t io_done;
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
	new->flink = (port_ctx) ctx->free_clients;
	ctx->free_clients = new;
    }
    marker += ctx->client_limit;	/* base for thread index numbers */
    UNLOCK_C_RTL
    /*
     * Create condition variable to synchronize I/O.
     */
    status = pthread_cond_init ( &io_done, pthread_condattr_default );
    /*
     * Determine operating mode.
     */
    permanent = tu_strncmp ( ctx->object_name, "sys$net", 8 );
    ctx->chan = ctx->mbx_chan = 0;
    if ( permanent ) {
	/*
	 * We are a permanent DECnet object, assign control channel with MBX.
	 */
        maxmsg = 300; bufquo = 500;
        ctx->status = LIB$ASN_WTH_MBX ( 
	    permanent ? &decnet_device : &decnet_sysnet,
	    &maxmsg, &bufquo, &ctx->chan, &ctx->mbx_chan );
	if ( (ctx->status&1) == 1 ) {
	    /*
	     * Assign successful,  declare network object, by name or number.
	     */
	    struct desc { long size; char *va; } dcb_dx, dcb_name_dx;
	    struct decl_block { 
	        unsigned char type;	/* NFB$C_DECLOBJ | NFB$C_DECLNAME */
	        long code;		/*  Object number | 0 */
	    } dcb;
	    char objname[20];
	    /*
	     * Initial declare block (dcb).
	     */
	    dcb_dx.size = sizeof(dcb);
	    dcb_dx.va = (char *) &dcb;
	    tu_strnzcpy ( objname, ctx->object_name, sizeof(objname)-1 );
	    dcb_name_dx.size = tu_strlen ( objname );
	    dcb_name_dx.va = objname;
	    /*
	     * Determine whether we want name or number and issue approprite
	     * QIO.
	     */
	    if ( (objname[0] >= '0') && (objname[0] <= '9') ) {
	        dcb.type = NFB$C_DECLOBJ;
	        dcb.code = atoi ( objname );
                pthread_mutex_lock ( &tcp_io );
	        status = SYS$QIO ( 0, ctx->chan, IO$_ACPCONTROL, &iosb,
		ts_synch_ast, &io_done, &dcb_dx, 0, 0, 0, 0, 0 );
	    } else if ( permanent ) {
	        dcb.type = NFB$C_DECLNAME;
	        dcb.code = 0;
                pthread_mutex_lock ( &tcp_io );
	        status = SYS$QIO ( 0, ctx->chan, IO$_ACPCONTROL, &iosb,
		    ts_synch_ast, &io_done, &dcb_dx, &dcb_name_dx, 0, 0, 0, 0 );
	    }
	    ctx->status = ts_synch_io ( status, &io_done, &iosb.status );
	} 
    } else {
	/*
	 * We are a transient (NETSERVER) process.
	 */
	int length;
	struct { short len, code; char *buffer; int *retlen, term; } item;
	/*
	 * Make fake connect message in msg structure and connect.
	 */
	msg.type = MSG$_CONNECT;
	msg.unit = msg.name_len = 0;
	item.len = 255;
	item.code = LNM$_STRING;
	item.buffer = &msg.info[1]; 
	item.retlen = &length;
	length = item.term = 0;
	ctx->status = SYS$TRNLNM ( 0, &lnm_file_dev, &decnet_sysnet, 0, &item );
	msg.info[0] = length;		/* string length */
        status = connect_accept ( ctx, &io_done, &msg, length + 6 );
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
    if ( (ctx->status&1) == 0 ) return ctx->status;
    ctx->status = 0;		/* No longer starting */
    /*
     * Read control mailbox and interpret messages.
     */
    if ( !permanent ) {
        struct timespec delta, abstime;
	/*
	 * Wait for created thread to complete.
	 */
       	delta.tv_sec = 60;
       	delta.tv_nsec = 0;
       	if ( 0 == pthread_get_expiration_np ( &delta, &abstime ) ) {
	    pthread_mutex_lock ( &tcp_ctl );
	    PUTLOG ( 0, "Rundown non-permanent connection !SL, !%D!/", 
		ctx->obj_num, 0 );
	    while ( ctx->client_count > 0 ) {
		ctx->connect_pending = 1;
        	if (0 > pthread_cond_timedwait( &ctx->client_available, 
			&tcp_ctl, &abstime ) ) break;
	    }
	    ctx->connect_pending = 0;
	    pthread_mutex_unlock ( &tcp_ctl );
        } else PUTLOG ( 0, "Time computation problem!/" );
	status = 1;
    } else for ( ; ; ) {
	int length, connect_accept();
	/*
	 * Read next mailbox message.
	 */
        client_chan = 0;
	/* printf("Reading from mailbox...\n"); */
        pthread_mutex_lock ( &tcp_io );
	status = SYS$QIO ( 0, ctx->mbx_chan, IO$_READVBLK, &iosb,
		ts_synch_ast, &io_done, &msg, sizeof(msg), 0, 0, 0, 0 );
	status = ts_synch_io ( status, &io_done, &iosb.status );
	if ( (status&1)==0 )
	    printf("Mailbox message status: %d, type: %d, unit: %d, len:%d\n", 
		  status, msg.type, msg.unit, msg.name_len );
	if ( (1&status) == 0 ) msg.type = 0;
	/*
	 * Take action based upon message type.
	 */
	switch ( msg.type ) {
	  case MSG$_CONNECT: 
	    status = connect_accept ( ctx, &io_done, &msg, iosb.count );
	    break;
	  case MSG$_NETSHUT:
	    /*
	     * DECnet being shutdown.
	     */
	    return 8372;
	  case MSG$_DISCON:
	  case MSG$_EXIT:
	    break;
	  default:
	    printf("Unknown message type: %d, ignored\n", msg.type );
	}
    }
    return status;
}

/***************************************************************************/
/* This routine handles allocating of new client context and sending $QIO
 * command to accept connection.  If connect succesful, new thread is created.
 */
static int connect_accept ( port_ctx ctx, pthread_cond_t *io_done,
	struct mailbox_message *msg, int msglen )
{
    int SYS$QIO(), i, status, denied;
    client_ctx new_client;
    struct { short status, count, d1, d2; } iosb;
    struct ncbdef reject_ncb, *ncb;
    /*
     * Validate access, note that we have tcp_ctl mutex while scanning list.
     */
    pthread_mutex_lock ( &tcp_ctl );
    if ( trusted_host_count > 0 ) {
	int info_len;
	char *remote_info;
	/*
	 * Search for entry in trusted list to match.
	 */
	denied = 1;
        info_len = msg->info[msg->name_len];
	remote_info = &msg->info[msg->name_len+1];
	for ( i = 0; i < trusted_host_count; i++ ) {
	   if ( info_len >= trusted_host_list[i].length ) {
	       if ( tu_strncmp ( remote_info, trusted_host_list[i].node_user,
			trusted_host_list[i].length ) == 0 ) {
		    denied = 0;	/* found match */
		    break;
		}
	    }
	}
    } else denied = 0;		/* no trusted list, allow access */
    /*
     * Allocate client block from free list.
     */
    if ( !denied ) new_client = ctx->free_clients; 
    else new_client = (client_ctx) 0;
    if ( ! new_client && !denied ) {
        /*
         * Give ourselves second chance to get client block, waiting
         * up to 30 seconds. 
         */
        struct timespec delta, abstime;
        delta.tv_sec = 30;
        delta.tv_nsec = 0;
        if ( 0 == pthread_get_expiration_np ( &delta, &abstime ) ) {
	    PUTLOG ( 0, "Thread list exhausted on port !SL, !%D!/", 
		ctx->obj_num, 0 );
	    ctx->connect_pending = 1;
            pthread_cond_timedwait 
	        ( &ctx->client_available, &tcp_ctl, &abstime );
	    ctx->connect_pending = 0;
	    new_client = ctx->free_clients;
        } else PUTLOG ( 0, "Time computation problem!/" );
    }
    if ( new_client ) {
        ctx->free_clients = (client_ctx) new_client->flink;
        ctx->client_count++;
        new_client->flink = (port_ctx) &ctx->client_list;
        new_client->blink = ctx->client_list.blink;
        ctx->client_list.blink = (port_ctx) new_client;
        new_client->blink->flink = (port_ctx) new_client;
	new_client->parent = ctx;
        new_client->status = 0;
        new_client->clients_remaining = 
		ctx->client_limit - ctx->client_count;
	ncb = &new_client->ncb;
    } else {
	ncb = &reject_ncb;
    }
    pthread_mutex_unlock ( &tcp_ctl );
    /*
     * Build ncb and determine function for accepting or rejecting connection
     */
    ncb->length = msg->info[msg->name_len];
    ncb->va = ncb->data;
    for ( i = 0; i < ncb->length; i++ ) {
	ncb->data[i] = msg->info[i+msg->name_len+1];
    }
    if ( new_client ) {
	/*
         * Accept connect and create new thread.
	 */
        int pthread_create(), SYS$ASSIGN();
        new_client->chan = 0;
        new_client->start = ctx->start;

        status = pthread_cond_init 
		( &new_client->io_done, pthread_condattr_default );

	status = SYS$ASSIGN ( &decnet_device, &new_client->chan, 0, 0 );
        pthread_mutex_lock ( &tcp_io );
	status = SYS$QIO ( 0, new_client->chan, IO$_ACCESS, 
		&new_client->iosb, ts_synch_ast, &new_client->io_done, 
		0, ncb, 0, 0, 0, 0 );
	status = ts_synch_io ( status, 
		&new_client->io_done, &new_client->iosb[0] );
	/*
	 * If connect accept worked, create thread to handle transaction.
	 */
    	if ( (status&1) == 1 ) status = pthread_create 
		( &new_client->thread, *(ctx->client_attr),
		ts_startup_client, new_client );
        if ( status == -1 ) perror ( "error creating client thread" );
    } else {
        pthread_mutex_lock ( &tcp_io );
	status = SYS$QIO ( 0, ctx->chan, IO$_ACCESS|IO$M_ABORT, 
		&iosb, ts_synch_ast, io_done, 0, ncb, 0, 0, 0, 0 );
	status = ts_synch_io ( status, io_done, &iosb.status );
	if ( denied )
            PUTLOG ( 0, "Connect reject, source not in trusted list!/" );
	else
            PUTLOG ( 0, "Connect reject, no free client contexts!/" );
    }
    return status;
}
/***************************************************************************/
/* Shell routine for a client thread, call startup routine store in contect.
 */
static int ts_startup_client ( client_ctx ctx )
{
    auto int auto_var;
    int status;

    pthread_setspecific ( client_key, ctx );
    ctx->stack_top = &auto_var;
    TRY {
    status = (*ctx->start) ( ctx, &ctx->ncb, 
		ctx->index, ctx->clients_remaining );
    }
    ENDTRY
    return status;
}
/***************************************************************************/
int ts_decnet_stack_used ()
{
    char auto_var;
    client_ctx ctx;

    if ( pthread_getspecific ( client_key, (pthread_addr_t *) &ctx ) )
	return -1;
    if ( !ctx ) return 0;
    return ((int)ctx->stack_top - (int)&auto_var);
}
/***************************************************************************/
/* Return information about the current connection, extracted from NCB.
 */
int ts_decnet_info ( char *taskname_num, char *remote_node, char *remote_user )
{
    int status, i;
    char *p;
    client_ctx ctx;
    status = pthread_getspecific ( client_key, (pthread_addr_t *) &ctx );
    if ( status != 0 ) return status;
    if ( ctx == (client_ctx) 0 ) return -1;

    /*
     * Extract remote node from NCB.
     */
    for ( p = ctx->ncb.data, i = 0; p[i]; i++ ) {
	remote_node[i] = p[i];
	if ( (p[i] == ':') && (p[i+1] == ':') ) {
	     remote_node[i+1] = ':';
	     i = i + 2;
	     break;
	}
    }
    remote_node[i] = '\0';
    p = &p[i];				/* skip extracted part */
    /*
     * Extract username.
     */
    if ( tu_strncmp(p, "\"0=", 3) == 0 ) for ( p = &p[3], i = 0; *p; i++ ) {
	if ( p[i] == '/' ) break;
	remote_user[i] = p[i];
    }
    remote_user[i] = '\0';
    /*
     * Skip link identifier and optional data.
     */
    if ( p[i] == '/' ) {
	i = i + 3;
	if ( p[i] != '"' )  i = i + 17;
    }
    /*
     * Extract taskname from destination if present.
     */
    p = &p[i];
    if ( p < &ctx->ncb.data[ctx->ncb.length-1] ) switch ( *p ) {
	case 0:
	    tu_strint ( (unsigned char) p[1], taskname_num );
	    break;
	case 1:
	    tu_strnzcpy ( taskname_num, &p[3], p[2] );
	    break;
	case 2:
	    tu_strnzcpy ( taskname_num, &p[7], p[6] );
	    break;
    }
    else taskname_num[0] = '\0';
    return 0;
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
    parent = ctx->parent;
	    /* Decrement counts */
	    --parent->client_count;
	    t = (client_ctx) ctx->flink;
	    t->blink = ctx->blink;
	    t = (client_ctx) ctx->blink; t->flink = ctx->flink;

    status = SYS$DASSGN ( ctx->chan );
    pthread_cond_destroy ( &ctx->io_done );	/* cleanup condition */
    pthread_detach ( &ctx->thread );
    if ( parent == &port_list ) 
	printf("Client block not found, port\n", ctx->unit );
    else {
	/* Place on free list, signal anybody waiting for client. */
	ctx->flink = (port_ctx) parent->free_clients;
	ctx->parent = &port_list;
	parent->free_clients = ctx;

	if ( parent->connect_pending ) {
	    PUTLOG ( 0, "Signalling client available: !%T!/", 0 );
	    pthread_cond_signal ( &parent->client_available );
	}
    }
    pthread_mutex_unlock ( &tcp_ctl );
}

/****************************************************************************/
static void port_rundown (port_ctx ctx )
{
    port_ctx t;
    client_ctx client;
    int count;

    LOCK_C_RTL
    printf("running down decnet listen port, ctx address: %d\n", ctx );
    UNLOCK_C_RTL
    if ( !ctx ) return;
    if ( ctx->status ) {
	/*
	 * Thread was in first phase of starting (declare_object is waiting
	 * for the startup status).  Emit the status.
	 */
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

    SYS$DASSGN ( ctx->chan );

    /* pthread_detach ( &ctx->thread ); */

    LOCK_C_RTL
    for ( client = ctx->free_clients; client; client = ctx->free_clients ) {
	ctx->free_clients = (client_ctx) client->flink;
	free ( client );
    }
    free ( ctx );
    UNLOCK_C_RTL
}
