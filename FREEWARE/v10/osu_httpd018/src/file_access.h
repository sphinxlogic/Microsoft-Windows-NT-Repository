/*
 * Thread-safe file access module.
 */
int tf_initialize();
void *tf_open(char *fname, char *mode, char errmsg[256] );
int tf_close( void *fptr );
int tf_read ( void *fptr, char *buffer, int bufsize );
int tf_last_error (void *fptr, int *last_errno, int *last_vms_cond );
int tf_getline ( void *fptr, char *buffer, int bufsize, int max_lines );
int tf_header_info ( void *fptr, int *size, unsigned *uic, 
		unsigned int *cdate, unsigned int *mdate );
char *tf_format_time ( unsigned long bintim, char *buffer );
unsigned long tf_decode_time ( char *string );
