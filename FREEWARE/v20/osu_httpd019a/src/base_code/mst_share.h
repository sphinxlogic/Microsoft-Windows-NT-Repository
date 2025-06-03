/*
 * Define globals and prototypes for use with MST_SHARE support routines
 * for dynamically loaded (i.e. shareable image) scriptserver threads.
 *
 * Redefine mst_ symbols to call indirectly though global pointers.
 */
#define mst_read (*mst_read_cb)
#define mst_write (*mst_write_cb)
#define mst_close (*mst_close_cb)
#define mst_exit (*mst_exit_cb)
#define mst_format_error (*mst_format_error_cb)
#define mst_connect (*mst_connect_cb)
/*
 * The follow globals will only be defined in modules that include pthread_np.h
 * prior to including mst_share.h
 */
#define mst_create_service_pool (*mst_create_service_pool_cb)
#define mst_register_service (*mst_register_service_cb)
/*
 * Now use the prototypes in message_service.h to declare the function pointers
 * by virtue of the defines above.
 */
#include "message_service.h"
/*
 * Define macros to override the global ints defined in message_service.h
 * to go through our own pointers, thus referencing the globals in the main
 * image.
 */

#define http_log_level *http_log_level_p
#define tlog_putlog (*tlog_putlog_cb)
int http_log_level;
int tlog_putlog ( int, char *, ... );

#define http_reentrant_c_rtl *http_reentrant_c_rtl_p
#define http_reentrant_vms_rtl *http_reentrant_vms_rtl_p
int http_reentrant_c_rtl, http_reentrant_vms_rtl;
/*
 * Mstshr_init is called by or as the shareable image's init routine 
 * (init keyword in config file service rule) to initalize the indirect
 * function/int pointers declared above.  Vector holds the addresses to use.
 */
int mstshr_init ( mst_linkage vector, char *info, char *errmsg );

/*
 * Define prototypes for routines contained to mst_share.c that provide
 * a CGI-like environment list to aid conversion from cgilib base programs
 * to dynamic MST's.
 */
struct mstshr_envbuf {		/* buffer for holding fake environ array */
    int count;			/* next envp index to use */
    int used;			/* Characters consumed */
    char *prolog[4];		/* Scriptserver protocol prolog */
    char *ptr[100];
    char buf[3072];		/* Storage for env strings */
};
int mstshr_cgi_symbols (
	mst_link_t cnx, 	/* I/O connection to client thread */
	char *info, 		/* MST connect() parameter passed by */
	struct mstshr_envbuf *env );	/* Structure to receive result */
char *mstshr_getenv ( char *name, struct mstshr_envbuf *env );
