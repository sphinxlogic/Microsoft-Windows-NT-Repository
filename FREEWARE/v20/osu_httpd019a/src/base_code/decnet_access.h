/*
 * Simple Decnet access routines for DECthreads.
 */
#ifdef VMS
int dnet_initialize ( );
int dnet_connect ( char *taskname, void **dptr );
int dnet_write ( void *dptr, char *buffer, int bufsize, int *written );
int dnet_read ( void *dptr, char *buffer, int bufsize, int *read );
int dnet_disconnect ( void *dptr );
int dnet_format_error ( int code, char *buffer, int bufsize );
#else
/*
 * For OSF, make dummy routines to substitute for DECnet.
 */
static int dnet_initialize() { return 1; }
static int dnet_dummy(void * a, ...) { return 0; }
typedef int (* efunc)();
static int dnet_format_error(int code, char *buffer, int bufsize )
   { tu_strnzcpy(buffer,"no DECnet",bufsize-1); return 1; }
#define dnet_connect dnet_dummy
#define dnet_write (efunc) dnet_dummy
#define dnet_read (efunc) dnet_dummy
#define dnet_disconnect (efunc) dnet_dummy
#endif
