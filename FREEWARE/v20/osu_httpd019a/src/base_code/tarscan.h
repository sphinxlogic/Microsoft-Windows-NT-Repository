/*
 * Define prototypes for routines to access tar file.
 */
int tar_init ( int max_streams );
void *tar_open ( char *archive, char errmsg[256] );
int tar_close ( void *volume );
int tar_set_module ( void *volume, char *name, int *size, char errmsg[256] );
int tar_read ( void *volume, char *buffer, int maxlen, int *length );
int tar_read_dir ( void *volume, char *name, int *size, unsigned *date );
