/*
 * This module provides non-transparent DECnet connections to DECThreads
 * modules.
 *
 * int dnet_initialize();
 * int dnet_connect ( char *taskname, *dhandle );
 * int dnet_write ( dnhandle fptr, char *buffer, int bufsize, int *written );
 * int dnet_read ( dnhandle fptr, char *buffer, int bufsize, int *read );
 * int dnet_disconnect ( dnhandle fptr );
 * int dnet_format_error ( int code, char *buffer, int bufsize );
 */
#include "pthread_np.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <descrip.h>
#include <iodef.h>

#include "tutil.h"
#include "decnet_access.h"	/* validate prototypes against actual */
#define DISABLE_ASTS pthread_mutex_lock(&ctx_list); /* disable AST delivery */
#define ENABLE_ASTS pthread_mutex_unlock(&ctx_list); /* enable AST delivier */
int SYS$DASSGN(), SYS$QIO();
/*
 * Global (module-wide) variables, Initialized by dnet_initialize.
 */
static $DESCRIPTOR(net_device,"_NET:");
struct dnet_iosb { short status; short length; long spare; };

struct connect_context {
    struct connect_context *flink, *blink;	/* Open connection list */
    int status;
    struct dnet_iosb iosb;
    short chan, fill;
    pthread_cond_t io_done;
};
typedef struct connect_context cnx_struct, *connection;

static int dnet_initialized, dnet_ef;
static connection free_connections;		/* Cache of free contexts. */
static pthread_mutex_t ctx_list;
static pthread_mutex_t dnet_io;			/* mutex for I/O cond. */
static pthread_key_t cnx_key;			/* Handle for pre-thread data */
/******************************************************************************/
/*
 * The following two routines are used to enabale a thread perform I/O 
 * without blocking the execution of other threads.  The thread must do
 * the following:
 *	1. Acquire io_done mutex.
 *	2  call SYS$QIO function specifying dnet_synch_ast as the
 *	   AST argument and the connection context as the parameter.
 *	3. call dnet_synch_io to get status and release the mutex.
 */
static int dnet_synch_ast ( connection ctx )
{
    int status;
    if ( ctx->status == 0 ) return 0;		/* on free list */
    /*
     * Signal any waiting threads.
     */
    status = pthread_cond_signal_int_np ( &ctx->io_done );
    return status;
}

static int dnet_synch_io ( int status, connection ctx )
{
    /*
     * Make sure operation is pending (success status).
     */
    if ( (status&1) == 1 ) {
	/*
	 * Loop until predicaate (ctx->iosb.status) is non-zero.
	 */
	do {
	    pthread_cond_wait ( &ctx->io_done, &dnet_io );
	} while ( ctx->iosb.status == 0 );
	status = (unsigned) ctx->iosb.status;
    }
    /*
     * Condition satisfied, unlock mutex.
     */
    pthread_mutex_unlock ( &dnet_io );
    return status;
}
/***************************************************************************/
/* Cleanup any decnet connection contexts associated with a thread.
 * The ctx argument is the first context in the list of contexts allocated
 * by the exiting thread.
 */
static void dnet_rundown ( connection ctx )
{
    connection cur, next;

    for ( cur = ctx; cur; cur = next ) {
	/*
	 * Deassign channel
	 */
	LOCK_C_RTL
	printf("/decnet/ running down dangling connection\n" );
	UNLOCK_C_RTL
	next = cur->flink;
	SYS$DASSGN ( cur->chan );
	cur->chan = 0;
	/*
	 * Place block on free list.
	 */
	DISABLE_ASTS
	cur->flink = free_connections;
	free_connections = cur;
	ENABLE_ASTS
    }
}
/****************************************************************************/
int dnet_initialize()
{
    int status, LIB$GET_EF(), pthread_keycreate();
    /*
     * Only perform initialization once.
     */
    pthread_lock_global_np();
    status = dnet_initialized;
    dnet_initialized = 1;		/* initialization started */
    pthread_unlock_global_np();
    if ( status ) return status;
    /*
     * Initialize decthreads objects.
     */
    free_connections = (connection) NULL;
    status = pthread_mutex_init ( &ctx_list, pthread_mutexattr_default );
    if ( status != 0 ) return 0;
    status = pthread_mutex_init ( &dnet_io, pthread_mutexattr_default );
    status = pthread_keycreate ( &cnx_key, dnet_rundown );
    if ( status == -1 ) perror ( "Error creating file access context key" );

    LOCK_VMS_RTL
    status = LIB$GET_EF ( &dnet_ef );
    UNLOCK_VMS_RTL
    return status;
}
/****************************************************************************/
/*
 * Establish connection to designated task.  Taskname must be valid
 * DECnet task specification:
 *
 *	node["user [password]"]::"[0=task|object]"
 */
int dnet_connect ( char *taskname, void **dptr )
{
    connection ctx;
    int i, status, SYS$ASSIGN(), length;
    struct { int length; char *data; } ncb_desc;
    char ncb[100];
    /*
     * Allocate a connection context block.
     */
    *dptr = (void *) NULL;
    DISABLE_ASTS
    ctx = free_connections;
    if ( ctx ) free_connections = ctx->flink;
    ENABLE_ASTS

    if ( ctx == (connection) NULL ) {
	LOCK_C_RTL
	ctx = (connection) malloc ( sizeof(cnx_struct) );
	UNLOCK_C_RTL
	if ( !ctx ) return 0;
	pthread_cond_init ( &ctx->io_done, pthread_condattr_default );

	ctx->flink = ctx->blink = (connection) NULL;
	ctx->status = 2;
	ctx->chan = 0;
    }
    /*
     * Build NCB and descriptor.  Replace final quote with
     * ncb connect/opt-data structure.
     */
    tu_strnzcpy ( ncb, taskname, 80 );
    length = tu_strlen ( ncb );
    if ( length > 0 ) ncb[length-1] = '/';
    ncb[length++] = 0;
    ncb[length++] = 0;			/* zero word (new connect */
    for ( i = 0; i < 17; i++ ) ncb[length++] = 0; 	/* null opt. data */
    ncb[length++] = '"';		/* closing quote */

    ncb_desc.length = length;
    ncb_desc.data = ncb;
    /*
     * Attempt connection to decnet object.
     */
    status = SYS$ASSIGN ( &net_device, &ctx->chan, 0, 0 );
    if ( (status&1) == 0 ) {
	DISABLE_ASTS 
	ctx->flink = free_connections; free_connections = ctx;
	ENABLE_ASTS
	return 0;
    }

    status = pthread_mutex_lock ( &dnet_io );
    ctx->status = 1;
    status = SYS$QIO ( dnet_ef, ctx->chan, IO$_ACCESS, &ctx->iosb,
		dnet_synch_ast, ctx,
		0, &ncb_desc, 0, 0, 0, 0 );
    status = dnet_synch_io ( status, ctx );
    if ( (status&1) == 1 ) {
	/*
	 * Add to context list for this thread.
	 */
	connection cur;
	if ( -1 == pthread_getspecific ( cnx_key, (void *) &cur ) ) return 20;
	ctx->blink = (connection) NULL;
	if ( cur ) cur->blink = ctx;
	ctx->flink = cur;
	pthread_setspecific ( cnx_key, ctx );
    } else {
	/* Failure */
	ctx->status = 0;
	SYS$DASSGN ( ctx->chan );
	DISABLE_ASTS
	ctx->flink = free_connections; free_connections = ctx;
	ENABLE_ASTS
	ctx = (connection) NULL;
    }
    /*
     * return status and context poitner to caller.
     */
    *dptr = (void *) ctx;
    return status;
}
/****************************************************************************/
int dnet_write ( void *dptr, char *buffer, int bufsize, int *written )
{
    int status;
    connection ctx;

    ctx = (connection) dptr;
    status = pthread_mutex_lock ( &dnet_io );
    status = SYS$QIO ( dnet_ef, ctx->chan, IO$_WRITEVBLK, &ctx->iosb,
		dnet_synch_ast, ctx,
		buffer, bufsize, 0, 0, 0, 0 );
    status = dnet_synch_io ( status, ctx );
    *written = (unsigned) ctx->iosb.length;
    return status;
}
/****************************************************************************/
int dnet_read ( void *dptr, char *buffer, int bufsize, int *read )
{
    int status;
    connection ctx;

    ctx = (connection) dptr;
    status = pthread_mutex_lock ( &dnet_io );
    status = SYS$QIO ( dnet_ef, ctx->chan, IO$_READVBLK, &ctx->iosb,
		dnet_synch_ast, ctx,
		buffer, bufsize, 0, 0, 0, 0 );
    status = dnet_synch_io ( status, ctx );
    *read = (unsigned) ctx->iosb.length;
    return status;
}
/****************************************************************************/
/****************************************************************************/
/* Perform a synchronous disconnect operation.
 */
int dnet_disconnect ( void *dptr )
{
    int status;
    connection ctx, first, cur;
    /*
     * Perform the disconnect.
     */
    ctx = (connection) dptr;
    if ( ctx->status == 0 ) return 20;
    status = pthread_mutex_lock ( &dnet_io );
    status = SYS$QIO ( dnet_ef, ctx->chan, IO$_DEACCESS|IO$M_SYNCH, &ctx->iosb,
		dnet_synch_ast, ctx,
		0, 0, 0, 0, 0, 0 );
    status = dnet_synch_io ( status, ctx );
    ctx->status = 0;
    SYS$DASSGN ( ctx->chan );
    /*
     * Verify the connection belongs to thread.
     */
    for ( pthread_getspecific(cnx_key, (void *) &cur); cur; cur = cur->flink ) {
	if ( cur == ctx ) break;
    }
    if ( cur != ctx ) {
	LOCK_C_RTL
	printf("Disconnect of unowned dnet connection\n");
	UNLOCK_C_RTL
    }
    /*
     * Remove from thread's connection list.
     */
    if ( ctx->flink ) ctx->flink->blink = ctx->blink;
    if ( ctx->blink ) ctx->blink->flink = ctx->flink;
    else pthread_setspecific ( cnx_key, ctx->flink );
    /*
     * Place control block onto free list.
     */
    DISABLE_ASTS
    ctx->flink = free_connections; free_connections = ctx;
    ENABLE_ASTS

    return status;
}
/****************************************************************************/
int dnet_format_error ( int code, char *buffer, int bufsize )
{
    int flags, status, SYS$GETMSG(), msglen, info;
    struct dsc$descriptor buf;

    buf.dsc$b_dtype = DSC$K_DTYPE_T;		/* text data */
    buf.dsc$b_class = DSC$K_CLASS_S;		/* fixed (Static) */
    buf.dsc$w_length = bufsize - 1;
    buf.dsc$a_pointer = buffer;
    flags = 0;

    pthread_lock_global_np();
    msglen = 0;
    status = SYS$GETMSG ( code, &msglen, &buf, flags, &info );
    pthread_unlock_global_np();
    if ( (status&1) == 1 ) buffer[msglen] = '\0';
    else buffer[0] = '\0';
    return status;
}
