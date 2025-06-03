/*  Inter-thread message passing substitute for DECnet task-to-task.
 *
 *	int mst_create_service_pool ( th_attr, int limit, int q_flag, int *id );
 *	int mst_register_service (char *name, start(), char *info, int pool_id);
 *	int mst_connect ( char *name, char *info, void **link );
 *	int mst_write ( link fptr, char *buffer, int bufsize, int *written );
 *	int mst_read ( link fptr, char *buffer, int bufsize, int *read );
 *	int mst_close ( link fptr );
 *	int mst_format_error ( int code, char *buffer, int bufsize );
 *	int mst_exit ( int *status );
 *
 * Initialization:
 *    Call mst_create_service_pool to reserve 1 or more pools of connection 
 *    structures for connecting to service threads with the thread attributes
 *    specified.  You are returned a unique pool ID number for the created
 *    pool.  If q_flag is set, clients will wait for next available connection
 *    if all reserved for that pool are in use.
 *
 *    Call mst_register to bind a service name with a thread pool and a
 *    start routine to be run by the service thread.
 *
 * Use:
 *    Client threads call mst_connect specifying a registered service name
 *    and are returned an opaque pointer to the new connection (link).  A 
 *    service thread is created and the associated start routine is called 
 *    with a complementary connection for communicating with the initiating 
 *    client.
 *
 *    Client thread and service thread exchange information using mst_read
 *    and mst_write.  The link between the 2 threads is not buffered and is
 *    half duplex.  Deadlocks (e.g. both threads reading) are detected and
 *    cause on one of the calls to fail.
 *
 *    Call mst_close to shutdown the connection and free resources. The client
 *    thread closing the connection DOES NOT force the service thread to exit,
 *    but it does cause pending/subsequent reads/writes to return
 *    MST_DISCONNECTED error status.  Thread exit will automatically close
 *    any connections left open by the thread.
 *
 * Author:	David Jones
 * Date:	25-MAR-1995  
 * Revised:     27-MAR-1995	Add TRY block around client start routine to
 *				catch exceptions.
 * Revised:	11-MAY-1995	Use memmove for nobuiltins option.
 * Revised:	23-MAY-1995	Fix bug that caused segmentation faults,
 *				cnx block not being added to closing list
 *				because disconnect_link returning DISCONNECT.
 *				(only happens if service thread fails to
 *				call mst_close).
 * Revised:	25-MAY-1995	Catch cma_e_thread_exits explicitly
 */

#include "pthread_np.h"
#include "tutil.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef VAX
#pragma builtins
#define __MEMMOVE(a,b,c) _MOVC3(c,b,a)
#else
#ifdef VMS
#pragma builtins
#include <builtins.h>
#else
#define nobuiltins 1
#endif
#endif
#include <string.h>
int tlog_putlog(), http_log_level;
#define TRACE if (http_log_level>1) tlog_putlog
/*
 * Define structures for managing connections:
 *   message_station
 *	Used to synchronize data transfer with another thread.
 *   connection
 *	Links together a pair of message stations.
 *   service_pool_def
 *	Manages a set of service threads and the connections that initiate
 *	them.  All threads in a service pool have similar thread attributes.
 *	A pool can have be either blocking (resource wait) or non-blocking,
 *      indicating whether a client thread will wait for a free service
 *	thread should none be available.
 *   service_def
 *	Binds a name to a service thread start routine and the service
 *      pool the thread is to allocate from.
 */
struct message_station {
    int state;			/* 0-inactive, 1-rblock, 2-wblock, 3-closed */
    int length;			/* size of buffer OR chars written */
    char *buffer;		/* only valid if state is 1 or 2 */
    int status;			/* predicate for blocked states */
};
typedef struct message_station *station;
#define STATION_CLEAR 0
#define STATION_READ_PENDING 1
#define STATION_WRITE_PENDING 2
#define STATION_CLOSED 3

struct connection {
    struct connection *flink, *blink;
    struct connection *surrogate;	/* if non-null, use inverse a,b of partner */
    pthread_cond_t transfer_completed;
    struct message_station a, b;
    struct service_def *server;
    int pool_id, avail, ndx;
    char info[256];
};
#define link mst_cnx_ptr
typedef struct connection *link;
#define mst_link_t link

#include "message_service.h"

struct service_def {
    struct service_def *next;
    char *info;
    char name[64];
    pthread_attr_t thread_attr;
    mst_start_routine_t *start;
    int pool_id;
};
typedef struct service_def *service;

struct service_pool_def {
    link free;				/* List of free connection structs */
    int id;				/* self address */
    int remaining;			/* Number on free list */
    int allocated;			/* Number of connections allocated */
    int limit;				/* Max number we can allocate */
    int resource_wait;
    int pending_connect;		/* predicate for pool waits */
    pthread_cond_t thread_available;
    pthread_attr_t thread_attr;		/* Attributes for pool's threads */
};
typedef struct service_pool_def *service_pool;

/*
 * Global data,
 */
static pthread_mutex_t redirect_io;		/* Protects global data */
static pthread_key_t open_link_key;		/* hook for created links */
static pthread_key_t accept_link_key;		/* created thread's link */
static pthread_once_t redirect_setup = pthread_once_init;
static link closing_list;			/* b side closed */
static service registered_services;
static service_pool *pool_list;			/* table of pool struct addr */
static struct connection failed_connection;
static int pool_list_size, pool_list_used;
static int connection_index;
/***************************************************************************/
/* Common routine for closing down a link, changing the 'a' station to the
 * closed state cleanly (aborting a pending read or write being done by the
 * partner thread).  Caller must lock redirect_io mutex.
 * The 'a' side station only is closed, 'b' must be closed by the other thread.
 */
static int disconnect_link ( station a, station b, pthread_cond_t *done )
{
    int status;
    if ( a->state != STATION_CLOSED ) {
	a->state = STATION_CLOSED;
	switch ( b->state ) {
	    case STATION_CLEAR:
	        status = MST_NORMAL;
		break;
	    case STATION_READ_PENDING:
	    case STATION_WRITE_PENDING:
		/*
		 * Abort the partners wait state.
		 */
		status = MST_NORMAL;
		b->length = 0;
		b->status = MST_DISCONNECT;
		b->state = STATION_CLEAR;	/* partner no longer blocked */
		if ( 0 > pthread_cond_signal ( done ) )
			printf("error signaling disconnect\n");
		break;
	    case STATION_CLOSED:
		/*
		 * Both stations now closed.
		 */
		status = MST_SHUTDOWN;
		break;
	}
    } else status = MST_NORMAL;
    return status;
}
/*************************************************************************/
/* Kernel routines for allocating/freeing connection structures,  caller is
 * assumed own redirect_io mutex.  Mutex will still be owned by this thread
 * upon return.  The pointer returned by allocate_connection actually
 * points to a pair of connection structures, the first is to be used
 * by a client thread and the second by the partner service thread.
 */
static link allocate_connection ( service_pool pool, int *remaining )
{
    link new;
    /*
     * See if connection available,
     */
    if ( !pool->free ) {
	/*
	 * Pool emtpy, see if we can have can add more to free list.
	 */
	if ( pool->allocated < pool->limit ) {
	    int alloc_size, i;
	    /*
	     * Add a chunk of up to 10 more pairs (each pair is allocated
	     * as a unit).
	     */
	    alloc_size = pool->limit - pool->allocated;
	    if ( alloc_size > 10 ) alloc_size = 10;
	    LOCK_C_RTL
	    pool->free = (link) malloc ( 
		sizeof(struct connection) * alloc_size * 2 );
	    UNLOCK_C_RTL
	    if ( pool->free ) {
		/*
		 *  Insert on free list.
		 */
		pool->allocated += alloc_size;
		pool->remaining += alloc_size;
		for ( i = 0; i < alloc_size; i++ ) {
		    connection_index++;
		    pool->free[i+i].ndx = connection_index;
		    pool->free[i+i].flink = &pool->free[i+i+2];
		    pthread_cond_init ( &pool->free[i+i].transfer_completed,
			pthread_condattr_default );
		    pool->free[i+i].pool_id = pool->id;
		}
		pool->free[(alloc_size-1)*2].flink = (link) 0;
	    }
	} else if ( pool->resource_wait ) {
	   /*
	    * wait for connection to become available.
	    */
	    TRACE(0,"/mst/ waiting for free cnx for pool !SL!/", pool->id );
	    while ( !pool->free ) {
		pool->pending_connect++;
		pthread_cond_wait ( &pool->thread_available, &redirect_io );
		--pool->pending_connect;
	    }
	    TRACE(0,"/mst/ Wait completed for cnx, addr=!SL!/", pool->free );
	}
    }
    new = pool->free;
    if ( new ) {
	pool->free = new->flink;
	new->avail = (--pool->remaining);
    }
    return new;
}

static int free_connection ( link cnx )   /* assumes redirect_io mutex held */
{
    int id;
    service_pool pool;
    pool = pool_list[cnx->pool_id];

    cnx->flink = pool->free;
    pool->free = cnx;
    pool->remaining++;
    if ( cnx->flink && pool->pending_connect )
	pthread_cond_signal ( &pool->thread_available );
    return pool->remaining;
}
/***************************************************************************/
/*
 * Cleanup links for terminating threads
 */
static void open_link_rundown ( pthread_addr_t key )
{
    link cnx, cur, next;
    pthread_mutex_lock ( &redirect_io );
    /*
     * Make sure links are closed.
     */
    cnx = (link) key;
    for ( cur = cnx; cur; cur = cur->flink ) {
	if (cur->a.state != STATION_CLOSED) 
		disconnect_link (&cur->a, &cur->b, &cur->transfer_completed);
    }
    /*
     * Place blocks on free list or closing list depending upon whether
     * the partner has closed.
     */
    for ( cur = cnx; cur; cur = next ) {
	next = cur->flink;
	if ( cur->b.state == STATION_CLOSED ) {
	    free_connection ( cur );
	} else {
	    cur->flink = closing_list;
	    cur->blink = (link) 0;
	    if ( closing_list ) closing_list->blink = cur;
	    closing_list = cur;
	}
    }
    pthread_mutex_unlock ( &redirect_io );
}
/***************************************************************************/
static void accept_link_rundown ( pthread_addr_t key )
{
    link cnx, cur, next;
    int status;
    /*
     * Make sure links are closed.
     */
    cnx = (link) key;
    cur = cnx->surrogate;		/* link struct used by client thread */
    if ( !cur ) return;
    pthread_mutex_lock ( &redirect_io );
    if ( (cur->b.state != STATION_CLOSED) ) {
	/*
	 * The service thread did not close this connection, close it now.
	 */
	cnx->surrogate = (link) 0;
	status = disconnect_link ( &cur->b, &cur->a, &cur->transfer_completed );
	if ( status == MST_SHUTDOWN ) {
	    /*
	     * In order to get this status, the client's connection must be 
	     * on the the closing list, move it to the free list.
	     */
	    if ( cur == closing_list ) {
		closing_list = cur->flink;
		if ( closing_list ) closing_list->blink = (link) 0;
	    } else {
		cur->blink->flink = cur->flink;
		if ( cur->flink ) cur->flink->blink = cur->blink;
	    }
	    free_connection ( cur );
	}
    } else if ( cur->a.state != STATION_CLOSED ) {
	/*
	 * We closed connection but client didn't.  Mark b side closed
	 * so the block will be freed when client does close his side.
	 */
	cnx->a.state = STATION_CLOSED;
	cnx->surrogate = (link) 0;
	cur->b.state = STATION_CLOSED;
    }
    pthread_mutex_unlock ( &redirect_io );
}
/***************************************************************************/
/* Do one-initialization of global variables. */
static void module_init ()
{
    int id, status, i;
    /* Create pthread objects. */
    pthread_mutex_init ( &redirect_io, pthread_mutexattr_default );
    pthread_keycreate ( &open_link_key, open_link_rundown );
    pthread_keycreate ( &accept_link_key, accept_link_rundown );
    /*
     * Create the initial service pool table.
     */
    connection_index = 0;
    pool_list_used = 1;
    pool_list_size = 5;
    LOCK_C_RTL
    pool_list = (service_pool *) malloc ( sizeof(struct service_pool_def *) *
		pool_list_size );
    if ( pool_list ) pool_list[0] = (service_pool) malloc
		( sizeof(struct service_pool_def) * pool_list_size );
    if ( pool_list[0] ) for ( i = 1; i < pool_list_size; i++ ) {
	pool_list[i] = &pool_list[0][i];
    }
    UNLOCK_C_RTL
    if ( !pool_list ) perror ( "allocation failure on service pool" );
    if ( !pool_list[0] ) perror ( "allocation failure on service pool" );
    /*
     * Create the default service pool.
     */
    pool_list[0]->free = (link) 0;	/* empty list */
    pool_list[0]->id = 0;
    pool_list[0]->remaining = 0;
    pool_list[0]->allocated = 0;
    pool_list[0]->limit = 10;
    pool_list[0]->resource_wait = 1;
    pool_list[0]->pending_connect = 0;
    pthread_cond_init ( &pool_list[0]->thread_available, 
	pthread_condattr_default );
    pool_list[0]->thread_attr = pthread_attr_default;
    /*
     * init remaining globals lists: limbo list and services.
     */
    failed_connection.a.state = failed_connection.b.state = STATION_CLOSED;
    failed_connection.surrogate = (link) 0;
    closing_list = (link) 0;
    registered_services = (service) 0;
}
/***************************************************************************/
/* Add a service pool to the service pool table, return ID.
 */
int mst_create_service_pool ( pthread_attr_t attr,
	int limit, int resource_wait, int *new_id )
{
    int id;
    /*
     * Allocate next slot in service pool table.
     */
    pthread_once ( &redirect_setup, module_init );
    pthread_mutex_lock ( &redirect_io );	/* lock access to global data */
    id = pool_list_used++;
    *new_id = id;
    if ( pool_list_used > pool_list_size ) {
	/*
	 * Expand table, double it.
	 */
	pool_list_size = pool_list_size * 2;
	LOCK_C_RTL
	pool_list = realloc ( pool_list, 
		sizeof(struct service_pool_def *) * pool_list_size );
	if ( pool_list ) pool_list[pool_list_used-1] = (service_pool) malloc
		( sizeof(struct service_pool_def) * 
		   (pool_list_size-pool_list_used+1) );
	if ( pool_list[pool_list_used-1] ) {
	    int i;
	    for ( i = pool_list_used; i < pool_list_size; i++ ) {
		pool_list[i] = &pool_list[0][i];
tlog_putlog (5, "/mst/ address of pool[!SL] structure: !SL!/", i, pool_list[i] );
	    }
	}

	UNLOCK_C_RTL
    }
    pthread_mutex_unlock ( &redirect_io );
    if ( !pool_list ) return MST_INSFMEM;
    /*
     * initialize the allocated pool structure.
     */
    pool_list[id]->free = (link) 0;	/* empty list */
    pool_list[id]->id = id;
    pool_list[id]->remaining = 0;
    pool_list[id]->allocated = 0;
    pool_list[id]->limit = limit;
    pool_list[id]->resource_wait = resource_wait;
    pool_list[id]->pending_connect = 0;
    pthread_cond_init ( &pool_list[id]->thread_available, 
	pthread_condattr_default );
    pool_list[id]->thread_attr = attr;
    return 1;
}

/***************************************************************************/
/* Register service routine and assign it a pool.
 */
int mst_register_service 
	( char *name, mst_start_routine_t start, char *info, int pool_id )
{
    int i;
    service srv;
    /*
     * Make sure service name isn't used, then allocate block.
     */
    pthread_once ( &redirect_setup, module_init );
    pthread_mutex_lock ( &redirect_io );
    if ( (pool_id < 0)  || (pool_id >= pool_list_used) ) {
	pthread_mutex_unlock ( &redirect_io );
	return MST_BADPARAM;
    }
    for ( srv = registered_services; srv; srv = srv->next ) {
	if ( tu_strncmp ( srv->name, name, 63 ) == 0 ) break;
    }
    pthread_mutex_unlock ( &redirect_io );
    if ( srv ) return MST_DUPLNAM;	/* already registered */
    LOCK_C_RTL
    srv = (service) malloc ( sizeof(struct service_def) );
    UNLOCK_C_RTL
    if ( !srv ) return MST_INSFMEM;
    /*
     * Initialize the service definition block.
     */
    tu_strnzcpy ( srv->name, name, 63 );
    srv->start = start;
    i = tu_strlen(info) + 1;
    LOCK_C_RTL
    srv->info = malloc ( i );
    UNLOCK_C_RTL
    if ( !srv->info ) return MST_INSFMEM;
    tu_strcpy ( srv->info, info );
    srv->pool_id = pool_id;
    /*
     * Add block to registered list.  Copy thread attribute from pool block
     * to service definition so that we don't have to get the redirect_io
     * mutex again when creating a thread.
     */
    pthread_mutex_lock ( &redirect_io );
    srv->next = registered_services;
    registered_services = srv;
    srv->thread_attr = pool_list[srv->pool_id]->thread_attr;
    pthread_mutex_unlock ( &redirect_io );
    return MST_NORMAL;
}
/***************************************************************************/
static void *server_shell ( void *arg )
{
    static int blah;
    struct connection *cnx;
    service server;
    pthread_t self;
    int status;
    char stack_top[4];
    /*
     * Initialize the dummy connection structure following our partner's.
     */
    cnx = (link) arg;
    server = cnx->server;
    cnx[1].a.state = cnx[1].b.state = STATION_CLOSED;
    cnx[1].flink = cnx[1].blink = (link) 0;
    cnx[1].surrogate = cnx;
    cnx[1].server = server;
    cnx[1].a.buffer = stack_top;	/* hack for mst_stack_used */
    /*
     * set rundown and call start routine.
     */
    pthread_setspecific ( accept_link_key, (pthread_addr_t) &cnx[1] );
    self = pthread_self();
    pthread_detach ( &self );
    TRY {
    status = (*server->start)( (void *) &cnx[1], server->name, cnx[1].info, 
	cnx->ndx, cnx->avail );
    }
    CATCH(cma_e_exit_thread) { /* Normal event, mst_exit called */ }
    CATCH_ALL {
        tlog_putlog(0,"Service!AZ/!SL, shell caught exception!/", server->name,
	    cnx->ndx );
	exc_report ( THIS_CATCH );
    }
    ENDTRY
    return (void *) &blah;
}
/***************************************************************************/
/*
 * create connection and thread to handle it.
 */
int mst_connect ( char *name, char *info, mst_link_t *handle )
{
    int status, remaining;
    service srv;
    link cnx, list_head;
    pthread_t thread;
    /*
     * find service definition, if found, attempt to get free connection.
     */
    *handle = &failed_connection;
    pthread_mutex_lock ( &redirect_io );
    for ( srv = registered_services; srv; srv = srv->next ) {
	if ( tu_strncmp ( name, srv->name, 63 ) == 0 ) {
	    cnx = allocate_connection ( pool_list[srv->pool_id], &remaining );
	    break;
	}
    }
    pthread_mutex_unlock ( &redirect_io );
    if ( !srv ) return MST_BADPARAM;		/* unregistered name */
    if ( !cnx ) return MST_INSFMEM;		/* allocation failure */
    /*
     * Initialize the connection structures for both client and service
     * threads.
     */

    cnx->a.state = cnx->b.state = STATION_CLEAR;
    cnx->surrogate = (link) 0;
    cnx->server = srv;
    cnx[1].surrogate = cnx;
    tu_strnzcpy ( cnx[1].info, info, sizeof(cnx->info)-1 );
    /*
     * Insert block in list of connects currently open by client so
     * that thread rundown can ensure cleanup.
     */
    pthread_getspecific ( open_link_key, (pthread_addr_t *) &list_head );
    cnx->flink = list_head;
    cnx->blink = (link) 0;
    if ( list_head ) list_head->blink = cnx;
    pthread_setspecific ( open_link_key, (pthread_addr_t) cnx );
    /*
     * create the service thread.
     */
    status = pthread_create (&thread, srv->thread_attr, server_shell, cnx);
    if ( status < 0 ) {
	return -1;
    }
    *handle = cnx;
    return MST_NORMAL;
}
/***************************************************************************/
/* Write a message from our buffer to partner thread's read buffer.
 */
int mst_write ( link cnx, char *buffer, int size, int *written )
{
    station a, b;
    int i;

    if ( cnx->surrogate ) {
	/* We are the 'slave' side, change cnx to point to the master
	 * connection but invert the channels.
	 */
	cnx = cnx->surrogate; 
	a = &cnx->b;
	b = &cnx->a;
    } else {
	a = &cnx->a;
	b = &cnx->b;
    }
    /*
     * while be hold the mutex, partner cannot update any of the connection
     * structures.
     */
    pthread_mutex_lock ( &redirect_io );
    /*
     * When called, the 'a' station will always be in either the clear
     * or closed states.
     */
    if ( a->state == STATION_CLOSED ) {
	/*
	 * We closed.
	 */
	a->status = MST_DISCONNECT;
	*written = 0;

    } else switch ( b->state ) {
	case STATION_CLEAR:
	    /*
	     * Wait for 'b' to read our buffer and signal us.
	     */
	    a->state = STATION_WRITE_PENDING;
	    a->length = size;
	    a->buffer = buffer;
	    a->status = 0;
	    do {
		if ( pthread_cond_wait ( &cnx->transfer_completed, 
			&redirect_io ) < 0 ) {
		   break;
		}
	    } while ( a->state != STATION_CLEAR );
	    *written = a->length;
	    break;
	case STATION_READ_PENDING:
	    /*
	     * 'b' is waiting for data, copy our buffer to b's buffer.
	     */
	    if ( size > b->length ) {
		size = b->length;
		a->status = b->status = MST_DATAOVERRUN;
	    } else {
		a->status = b->status = MST_NORMAL;
		b->length = size;
	    }
#ifdef nobuiltins
	    memmove ( b->buffer, buffer, size < 0 ? 0 : size );
	    /* for ( i = 0; i < size; i++ ) b->buffer[i] = *buffer++; */
#else
	    __MEMMOVE ( b->buffer, buffer, size < 0 ? 0 : size );
#endif
	    *written = size;
	    /*
	     * Clear pending state and signal 'b' so it can proceed.
	     */
	    b->state = STATION_CLEAR;		/* no longer blocked */
	    pthread_cond_signal ( &cnx->transfer_completed );
	    break;
	case STATION_WRITE_PENDING:
	    /*
	     * Both sides of connection trying to write, bad news.
	     */
	    a->status = MST_DEADLOCK;
	    *written = 0;
	    break;
	case STATION_CLOSED:
	    /*
	     * Partner closed connection.
	     */
	    a->status = MST_DISCONNECT;
	    *written = 0;
	    break;
	default:
	    a->status = MST_INVSTATE;
    }
   pthread_mutex_unlock ( &redirect_io );
    return a->status;
}
/***************************************************************************/
/* Read a message into our buffer to partner thread's write buffer.
 */
int mst_read ( link cnx, char *buffer, int size, int *written )
{
    struct message_station *a, *b;
    int i;
link tmp = cnx;
    if ( cnx->surrogate ) {
	/* We are the 'slave' side, change cnx to point to the master
	 * connection but invert the channels.
	 */
	cnx = cnx->surrogate; 
	a = &cnx->b;
	b = &cnx->a;
    } else {
	a = &cnx->a;
	b = &cnx->b;
    }
    /*
     * while be hold the mutex, partner cannot update any of the connection
     * structures.
     */
    pthread_mutex_lock ( &redirect_io );
    /*
     * When called, the 'a' station will always be in either the clear
     * or closed states.
     */
    if ( a->state == STATION_CLOSED ) {
	/*
	 * We closed.
	 */
	a->status = MST_DISCONNECT;
	*written = 0;

    } else switch ( b->state ) {
	case STATION_CLEAR:
	    /*
	     * Wait for 'b' to write to our buffer and signal us.
	     */
	    a->state = STATION_READ_PENDING;
	    a->length = size;
	    a->buffer = buffer;
	    a->status = 0;
	    do {
		if ( pthread_cond_wait ( &cnx->transfer_completed, 
			&redirect_io ) < 0 ) {
		printf("/mst_read/ cond wait failed\n");
		   break;
		}
	    } while ( a->state != STATION_CLEAR );
	    *written = a->length;
	    break;
	case STATION_READ_PENDING:
	    /*
	     * Both sides of connection trying to read, bad news.
	     */
	    a->status = MST_DEADLOCK;
	    *written = 0;
	    break;

	case STATION_WRITE_PENDING:
	    /*
	     * 'b' is sending us data, copy buffer to our's
	     */
	    if ( size < b->length ) {
		b->length = size;
		a->status = b->status = MST_DATAOVERRUN;
	    } else {
		a->status = b->status = MST_NORMAL;
		size = b->length;
	    }
#ifdef nobuiltins
	    memmove ( buffer, b->buffer, size < 0 ? 0 : size );
	    /* for ( i = 0; i < size; i++ ) *buffer++ = b->buffer[i]; */
#else
	    __MEMMOVE ( buffer, b->buffer, size < 0 ? 0 : size );
#endif
	    *written = size;
	    /*
	     * Clear pending state and signal 'b' so it can proceed.
	     */
	    b->state = STATION_CLEAR;		/* no longer blocked */
	    pthread_cond_signal ( &cnx->transfer_completed );
	    break;

	case STATION_CLOSED:
	    /*
	     * Partner closed connection.
	     */
	    a->status = MST_DISCONNECT;
	    *written = 0;
	    break;
	default:
	    a->status = MST_INVSTATE;
    }
    pthread_mutex_unlock ( &redirect_io );
    return a->status;
}

/****************************************************************************/
/* Shutdown one side of connection.
 */
int mst_close ( link cnx )
{
    int status;
    station a, b;
    pthread_cond_t *transfer_completed;
    link list_head, sur;

    if ( cnx->surrogate ) {
	/* We are the 'slave' side, change cnx to point to the master
	 * connection but invert the channels.
	 */
	transfer_completed = &cnx->surrogate->transfer_completed;
	a = &cnx->surrogate->b;
	b = &cnx->surrogate->a;
	list_head = (link) 0;
    } else {
	/*
	 * We have the master copy, attempt to locate it on our per-thread
	 * list and remove it.
	 */
	link cur; 
	pthread_getspecific ( open_link_key, (pthread_addr_t *) &list_head);
	if ( list_head &&( list_head == cnx) ) {
	    /* We are first in list */
	    if ( cnx->flink ) cnx->flink->blink = (link) 0;
	    pthread_setspecific ( open_link_key, (pthread_addr_t) cnx->flink );
	} else {
	    for ( cur = list_head; cur; cur = cur->flink ) {
		if ( cur->flink == cnx ) {
		    cur->flink = cnx->flink;
		    if ( cur->flink ) cur->flink->blink = cur;
		    break;
		}
	    }
if ( !cur ) tlog_putlog(0,"Error in mst_close(), not found on client list!/");
	    if ( !cur ) list_head = (link) 0;	/* not found */
	}

	transfer_completed = &cnx->transfer_completed;
	a = &cnx->a;
	b = &cnx->b;
    }
    /*
     * Call disconnect_link to change our state closed and abort any I/Os
     * the partner thread has in progress.
     */
    pthread_mutex_lock ( &redirect_io );
    status = disconnect_link ( a, b, transfer_completed );
    if ( status == MST_SHUTDOWN ) {
	/*
	 * both sides now shut down, change to normal status.
	 */
	status = MST_NORMAL;
	if ( cnx->surrogate ) {
	    /*
	     * We are the service side of a channel and the client closed
	     * first, leaving it's block on the closing_list list.  Move
	     * the client's cnx to the free list.
	     */
	    sur = cnx->surrogate;
	    if ( sur != closing_list ) {
		if ( sur->flink ) sur->flink->blink = sur->blink;
		sur->blink->flink = sur->flink;
	    } else {
		closing_list = sur->flink;
		if ( closing_list ) closing_list->blink = (link) 0;
	    }
	    free_connection ( sur );
	    /*
	     * Convert the callers link structure to a dormant one.
	     */
	    cnx->a.state = cnx->b.state = STATION_CLOSED;
	    cnx->surrogate = (link) 0;
	} else {
	    /*
	     * We are client side of link and server already closed,
	     * put block back on free list.
	     */
	    free_connection ( cnx );
	}
    } else if ( (status == MST_NORMAL) && list_head ) {
	/*
	 * We are the client side of the connection and service thread
	 * still open, put block on closing list (limbo) and when partner
	 * thread closes his side it will be freed.
	 */
	cnx->flink = closing_list;
	cnx->blink = (link) 0;
	if ( closing_list ) closing_list->blink = cnx;
	closing_list = cnx;
    }
    pthread_mutex_unlock ( &redirect_io );
    return status;
}
/***************************************************************************/
/* Return string describing error code returned by mst_ routine. */
int mst_format_error ( int code, char *buffer, int bufsize )
{
    static struct { int code; char *description; } errlist[] =  {
	{ MST_NORMAL, "Normal, successful completion" },
	{ MST_DISCONNECT, "Partner thread disconnected link" },
	{ MST_DATAOVERRUN, "Receiving buffer too small for data sent" },
	{ MST_SHUTDOWN, "Attempt to close already closed link" },
	{ MST_DEADLOCK, "partner thread blocked attempting same operation" },
	{ MST_INVSTATE, "Bugcheck, internal state inconsistent" },
	{ MST_DUPLNAM, "Requested service name already in use" },
	{ MST_INSFMEM, "Memory allocation failure" },
	{ MST_BADPARAM, "Bad parameter" },
	{ 0, (char *) 0 } /* flag end of list */
    };
    int i;

    for ( i = 0; errlist[i].code != code; i++ ) {
	if ( errlist[i].code == 0) break;
    }
    if ( errlist[i].description ) {
	tu_strnzcpy ( buffer, errlist[i].description, bufsize-1 );
	return MST_NORMAL;
    } else {
	tu_strnzcpy ( buffer, "Unknown error code", bufsize-1 );
	return 0;
    }
}
/****************************************************************************/
/* Exit thread with status.  Should never return.
 */
int mst_exit ( int *status )
{
    pthread_exit ( (pthread_addr_t *) status );
    return -1;
}
/*****************************************************************************/
/* Hack to let service thread measure its stack consumption.
 */
/***************************************************************************/
int mst_stack_used ()
{
    char auto_var;
    link cnx;

    if ( pthread_getspecific ( accept_link_key, (pthread_addr_t *) &cnx ) )
	return -1;
    if ( !cnx ) return 0;
    return ((int)cnx->a.buffer - (int)&auto_var);
}
