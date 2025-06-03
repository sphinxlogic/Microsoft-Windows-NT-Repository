/*
 * Define routines and return values for Message-based Service Thread facility.
 */
#define MST_NORMAL 1
#define MST_SHUTDOWN 8332
#define MST_DEADLOCK 3954
#define MST_DISCONNECT 8428
#define MST_INVSTATE 676
#define MST_DATAOVERRUN 2104
#define MST_DUPLNAM 148
#define MST_INSFMEM 292
#define MST_BADPARAM 20
/*
 * Rig up a macro so we can have client_ctx pointer opaque to the calling
 * modules.
 */
#ifndef mst_link_t
#define mst_link_t void *
#endif

/*
 * Only declare the initialization routines if the pthread_np module has been 
 * included.  This saves us from have a false dependancy on pthread_np.h
 * by modules that only are using the connect/read/write routines.
 */
#ifdef loaded_pthread_np
int mst_create_service_pool ( 
	pthread_attr_t attr, 	/* thread attributes (set noinherit) */
	int limit, 		/* Max service threads in pool or 0 */
	int q_flag, 		/* If set, clients wait for avail. thread */
	int *id );              /* tag for created pool */

typedef int mst_start_routine_t (
	mst_link_t link,	/* connection handle */
	char *service_name,	/* name client connected to */
	char *info, 		/* additional string supplied by client */
	int ndx,		/* thread index number */
	int avail );		/* Available threads remaining in pool */

int mst_register_service ( 
	char *name,		/* Name clients use to connect to service */
	mst_start_routine_t start,
	char *info, 		/* Additional parameter for service */
	int pool_id );		/* thread pool to use or 0 (default pool) */
#endif
int mst_connect ( char *name, char *info, mst_link_t *cnx );
int mst_write ( mst_link_t cnx, char *buffer, int size, int *written );
int mst_read ( mst_link_t cnx, char *buffer, int size, int *written );
int mst_close ( mst_link_t cnx );
int mst_format_error ( int code, char *buffer, int bufsize );
int mst_exit ( int *status );
/*
 * Define structure for use by dynamically loaded shareable images.
 */
#define MST_CB_VERSION 2
struct mst_callback_vector {
    int version;			/* version of cb vector format */
    int *trace_level;			/* address of http_log_level */
    int *reentrant_c_rtl, *reentrant_vms_rtl;
    int (*putlog)(int, char*, ...);	/* address of tlog_putlog() */
    int (*read)(), (*write)();		/* Callbacks for mst_read, mst_write */
    int (*close)(), (*exit)();
    int (*format_error)(), (*connect)();
    int (*create_service_pool)(), (*register_service)();
};
typedef struct mst_callback_vector *mst_linkage;
