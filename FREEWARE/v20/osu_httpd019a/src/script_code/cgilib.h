/*
 * Prototypes for cgi support routines, or standard script substitutes if
 * not useing real CGI.
 */
#ifdef NOCGILIB
#define cgi_init(a,b)  1
#define cgi_info(a) getenv(strcpy(&cgi_info_buf[4], a)-4)
static char cgi_info_buf[64] = { 'W', 'W', 'W', '_' };
#define cgi_printf printf
static FILE *cgi_content_file() { return stdin; }
#else
int cgi_init ( int argc, char **argv );
char *cgi_info ( char *name );
int cgi_write ( char *string, int length );
int cgi_printf ( const char *ctlstr, ... );
int cgi_read ( char *buffer, int bufsize );
int cgi_set_dcl_env ( char *prefix );
FILE *cgi_content_file();

int cgi_init_env ( int argc, char **argv );
int cgi_begin_output ( int rundown_on_exit );
int cgi_translate_path ( char *path, int check_protection,
	char *buffer, int bufsize, int *length);
#endif
