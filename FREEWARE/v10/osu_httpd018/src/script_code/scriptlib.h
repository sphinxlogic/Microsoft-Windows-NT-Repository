/*
 * Prototypes for script-side network support functions.
 */
int net_link_open();
int net_link_close();
int net_link_read ( void *buffer, int bufsize, int *read );
int net_link_write ( void *buffer, int bufsize );
int net_link_query ( void *tag, void *buffer, int bufsize, int *read );
int net_link_printf ( char *ctlstr, ... );
char *net_unescape_string ( char *string, int *length );
int net_link_set_rundown ( char *tag );
int net_link_set_mode ( int mode );
