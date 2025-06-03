/*
 * Header file for defining prototypes for DECnet server module.
 *
 * Rig up a macro so we can have client_ctx pointer opaque to the calling
 * modules.
 */
#ifndef ts_client_ctx
#define ts_client_ctx void *
struct ncbdef { long length; char *va; char data[256]; };
#endif
/*
 * Since declare_decnet_object can only used by modules that include 
 * pthread_np.h, only define it if such is the case.  This saves us from
 * having a false dependancy on pthread_np.h by every module that uses
 * tserver_decnet.
 */
#ifdef loaded_pthread_np
int ts_declare_decnet_object (
    char *object_name_num,	/* Object name (or number) */
    int client_limit, 		/* Limit on concurrent client threads allowed */
    pthread_attr_t *client_attr, /* Thread attributes for client threads */
    pthread_t *control_thread, 	/* Thread that listens for connects */
    int start( ts_client_ctx ctx, struct ncbdef *ncb,int ndx, int available) );
#endif
int ts_decnet_write ( ts_client_ctx ctx, char *buffer, int length );
int ts_decnet_read ( ts_client_ctx, char *buffer, int maxlen, int *length );
int ts_decnet_close ( ts_client_ctx ctx );
int ts_decnet_stack_used();	/* debugging routine */
int ts_decnet_info ( char *taskname_num, char *remote_node, char *remote_user);
/* int ts_set_manage_port ( int port_num, unsigned int host_address,
	int callback(ts_client_ctx ctx, short port, int *shutdown_time) ); */
int ts_set_logging ( int callback(int, char *, ...) );
int ts_set_access ( char *log_name );
