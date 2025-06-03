/*
 * Header file for defining prototypes for TCP/IP server module.
 *
 * Rig up a macro so we can have client_ctx pointer opaque to the calling
 * modules.
 */
#ifndef ts_client_ctx
#define ts_client_ctx void *
#endif
/*
 * Since declare_tcp_port can only used by modules that include 
 * pthread_np.h, only define if such is the case.  This saves us from
 * having a false dependancy on pthread_np.h by every module that uses
 * tserver_tcp.
 */
#ifdef loaded_pthread_np
int ts_declare_tcp_port ( int port_num, 
	int client_limit,
	pthread_attr_t *client_attr,
	pthread_t *control_thread,
	int start( ts_client_ctx ctx, short port,
		unsigned char *rem_address, int ndx, int available) );
#endif
int ts_tcp_write ( ts_client_ctx ctx, char *buffer, int length );
int ts_tcp_read ( ts_client_ctx, char *buffer, int maxlen, int *length );
int ts_tcp_close ( ts_client_ctx ctx );
int ts_tcp_stack_used();	/* debugging routine */
int ts_tcp_info ( int *local_port, int *remote_port, 
	unsigned int *remote_address );
char *ts_tcp_remote_host();
int ts_set_manage_port ( int port_num, unsigned int host_address,
	int callback(ts_client_ctx ctx, short port, int *shutdown_time) );
int ts_set_logging ( int callback(int, char *, ...) );
int ts_set_local_addr ( char * );
