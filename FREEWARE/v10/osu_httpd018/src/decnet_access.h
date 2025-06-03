/*
 * Simple Decnet access routines for DECthreads.
 */
int dnet_initialize ( );
int dnet_connect ( char *taskname, void **dptr );
int dnet_write ( void *dptr, char *buffer, int bufsize, int *written );
int dnet_read ( void *dptr, char *buffer, int bufsize, int *read );
int dnet_disconnect ( void *dptr );
int dnet_format_error ( int code, char *buffer, int bufsize );
