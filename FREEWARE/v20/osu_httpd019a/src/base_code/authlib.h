/*
 * Define prototypes for exported/imported functions in authlib.c.
 */
int auth_server ( int argc, char **argv, char *log_line, int code_count, ... );

int auth_basic_authorization ( char *astring,
	char *user, int ulen, char *password, int plen, int upcase );

int auth_pubkey_authorization ( char *astring, char *privkey,
	char *user, int ulen, char *password, int plen, int upcase );

typedef int auth_callback ( int local_port, int remote_port, 
	unsigned char *remote_address, char *method, char *setup_file,
	char *ident, char *authorization, char **response, char **log );

