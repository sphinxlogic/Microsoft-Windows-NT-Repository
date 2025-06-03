/*
 * Define support routines for porting CGI (Common Gateway Interface)
 * conforming programs to the DECthreads script environment.
 *
 * Routines:
 *    int cgi_init ( int argc, char **argv );
 *		Initialize support routines.  Command line arguments will
 *		be retrieved.
 *
 *    char *cgi_info ( char *name );
 *		This routine returns environment and request information
 *		for the current request.  If run in the CERN server
 *		environment, this routine simply performs a get
 *
 *    int cgi_write ( char *string, int length );
 *		Send output.
 *
 *    int cgi_printf ( char *ctlstr, ... );
 *
 *    int cgi_read ( char *buffer, int bufsize );
 *
 *    int cgi_set_dcl_env ( char *prefix );
 *
 *    FILE *cgi_content_file();
 *
 *  Author:	David Jones
 *  Revised:	2-JUN-1994		! don't decode query_string (unless
 *					! module compiled with DECODE_QUERY
 *  Revised:	27-JUN-1994		! corrected cgi_read to use contentf.
 *  Revised:	19-JUL-1994		! Fixed bad parse of content_length.
 *  Revised:	27-SEP-1994		! added cgi_set_dcl_env() routine.
 *  Revised:    18-OCT-1994		! Switch to <DNETID2> to get host name.
 *  Revised:    20-DEC-1994		! Fixed bug in allocating scriptname
 *					! storage, wrong size being used.
 *  Revised:	14-JAN-1995		! Added missing REMOTE_PORT to env
 *					! table,  fixed length allocation in
 *					! fetch value.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <descrip.h>
#include <libclidef.h>
#define strerror strerror_1
#include <string.h>
#undef strerror
char *strerror ( int errnum, ... );
#include <ctype.h>
#include <errno.h>
#include "cgilib.h"
#include "scriptlib.h"
/*
 * Define static structure to hold environment information expect by CGI
 * programs.
 */
struct envdef { char *name, *value; int state; };

#define ENV_LIMIT 200
#define NET_IO_SIZE 1000
static int content_length=0, env_used = 19;
static FILE *contentf;
static struct envdef env[ENV_LIMIT] = {
 /* Request independent variables */
 { "SERVER_SOFTWARE", "", 0 },
 { "SERVER_NAME", "", 0 },
 { "GATEWAY_INTERFACE", "CGI/1.0", 1 },

 /* Request-specific variables */
 { "SERVER_PROTOCOL", "", 0 },
 { "SERVER_PORT", "", 0 },
 { "REQUEST_METHOD", "", 0 },
 { "PATH_INFO", "", 0 },
 { "PATH_TRANSLATED", "", 0 },
 { "SCRIPT_NAME", "", 0 },
 { "SCRIPT_PATH", "", 0 },
 { "QUERY_STRING","", 0 },
 { "REMOTE_USER", "", 0 },
 { "REMOTE_ADDR", "", 0 },
 { "REMOTE_PORT", "", 0 },
 { "REMOTE_HOST", "", 0 },
 { "AUTH_TYPE", "", 0 },
 { "REMOTE_IDENT", "", 0 },
 { "CONTENT_TYPE", "", 0 },
 { "CONTENT_LENGTH", "", 0 }
};

static int env_index ( char *name )
{
    int i;
    for (i = 0; i < env_used; i++) if (0==strcmp ( name, env[i].name)) {
	return i;
    }
    return ENV_LIMIT;
}

/* Query server for tag and place result in environment array 
 */
static int fetch_value ( char *name, char *tag, int *length, char **value )
{
    int status, ndx;
    char buffer[4096];
#ifdef DEBUG
printf("/cgilib/ fetching value %s (%s)\n", name, tag );
#endif
    status = net_link_query ( tag, buffer, sizeof(buffer), length );
#ifdef DEBUG
printf("      status of query: %d, len: %d\n", status, *length );
#endif
    if ( (status&1) == 0 ) return status;
    if ( *length > 0 ) {
	ndx = env_index ( name );
#ifdef DEBUG
printf("      index for %s is %d\n", name, ndx );
#endif
	if ( ndx >= ENV_LIMIT ) return 20;
	env[ndx].value = *value = malloc ( (*length) + 1 );
	strncpy ( *value, buffer, *length );
	env[ndx].value[*length] = '\0';
	env[ndx].state = 1;
    }
    return status;
}

static int load_translation ( char *name, char *tag, char *arg )
{
    int status, ndx, length;
    char *value, buffer[4096];
    status = net_link_write ( tag, strlen(tag) );
    if ( (status&1) == 0 ) return status;
    status = net_link_query ( arg, buffer, sizeof(buffer), &length );
    if ( (status&1) == 0 ) return status;
    if ( length > 0 ) {
	ndx = env_index ( name );
	if ( ndx >= ENV_LIMIT ) return 20;
	env[ndx].value = value = malloc ( (length) + 1 );
	strncpy ( value, buffer, length );
	env[ndx].value[length] = '\0';
	env[ndx].state = 1;
    }
    return status;
}
/***************************************************************************/
/* Return copy of content_file pointer.
 */
FILE *cgi_content_file() { return contentf; }
/***************************************************************************/
/* Initialize CGI environment array.  Return value is VMS condition code.
 */
int cgi_init ( int argc, char **argv )
{
    int status, ndx, i, colon, length;
    char *label, *value, buffer[4096];
    /*
     * Establish connection to server.
     */
    status = net_link_open();
    if ( (status&1) == 0 ) return status;
    /*
     * Assume program invoked with arguments: method url protocol
     */
    if ( (ndx = env_index ( "REQUEST_METHOD" )) < ENV_LIMIT ) {
	env[ndx].state = 1;
	if ( argc > 1 ) env[ndx].value = argv[1];
    }
    if ( (ndx = env_index ( "SCRIPT_NAME" )) < ENV_LIMIT ) {
	if ( argc > 2 ) {
	    /* Duplicate argv[2] string. */
	    env[ndx].value = malloc ( strlen(argv[2])+1 );
	    strcpy ( env[ndx].value, argv[2] );
	    env[ndx].state = 1;
	}
    }
    if ( (ndx = env_index ( "SERVER_PROTOCOL" )) < ENV_LIMIT ) {
	env[ndx].state = 1;
	if ( argc > 3 ) env[ndx].value = argv[3];
    }
    /*
     * Ask server for its version and connection information.
     */
    status = fetch_value ( "SERVER_SOFTWARE", "<DNETID2>", &length, &value );
    if ( (status&1) == 1  ) value = strchr(value, ' ');
    else value = NULL;
    if ( value ) {
	*value++ = '\0';
	ndx = env_index ( "SERVER_NAME" );
	env[ndx].state = 1; env[ndx].value = value;
	value = strchr(value, ' ');
    }
    if ( value ) {
	*value++ = '\0';
	ndx = env_index ( "SERVER_PORT" );
	env[ndx].state = 1; env[ndx].value = value;
	value = strchr(value, ' ');
    }
    if ( value ) {
	*value++ = '\0';
	ndx = env_index ( "REMOTE_PORT" );
	env[ndx].state = 1; env[ndx].value = value;
	value = strchr(value, ' ');
    }
    if ( value ) {
	int addr;
	char *nextval;
	*value++ = '\0';
	ndx = env_index ( "REMOTE_ADDR" );
	nextval = strchr(value,' ');		/* find end of string */
	if ( nextval ) *nextval = '\0';		/* truncate it for atoi() */
	addr = atoi ( value );
	env[ndx].state = 1; env[ndx].value = malloc(16);
	sprintf ( env[ndx].value, "%d.%d.%d.%d", (addr&255),
	    ((addr>>8)&255), ((addr>>16)&255), ((addr>>24)&255) );
	value = nextval;
    }
    if ( value ) {
	*value++ = '\0';
	ndx = env_index ( "REMOTE_USER" );
	/*
	 * Only set value if string is non-null.
	 */
	if ( *value != ' ' ) env[ndx].state = 1; env[ndx].value = value;
	value = strchr(value,' ');		/* find end of string */
    }
    if ( value ) {
	*value++ = '\0';
	ndx = env_index ( "REMOTE_HOST" );
	env[ndx].state = 1; env[ndx].value = value;
	value = strchr(value,' ');		/* Find end of string */
	if ( value ) *value = '\0';		/* Terminate it */
    } else {
	/* Fallback remote host to REMOTE_ADDR */
	int andx;
	andx = env_index ( "REMOTE_ADDR" );
	if ( env[andx].state ) {
	    ndx = env_index ( "REMOTE_HOST" );
	    env[ndx].state = 1; env[ndx].value = env[andx].value;
	}
    }
    /*
     * Ask for name of script being executed and derive path_info and
     * script name from that.
     */
    status = fetch_value ( "SCRIPT_PATH", "<DNETPATH>", &length, &value );
    if ( ((status&1) == 1) && (strncmp(argv[2], value, length )==0) ) {
	/*
	 * add path_info entry.
	 */
	char *p;
	if ( (ndx = env_index ( "PATH_INFO" )) < ENV_LIMIT ) {
	    env[ndx].state = 1;
	    for ( p = &argv[2][length]; *p && (*p != '/'); p++ );
	    env[ndx].value = p;

	    load_translation ( "PATH_TRANSLATED", "<DNETXLATE>", p );

	}
	if ( (ndx = env_index ( "SCRIPT_NAME" )) < ENV_LIMIT ) {
	        if ( strlen(env[ndx].value) > length ) {
	        for ( p = &env[ndx].value[length]; *p && (*p != '/'); p++ );
	        *p = '\0';		/* Truncate script name */
	    }
	}
    }
    /*
     * Get query string and convert escaped characters.
     */
    status = fetch_value ( "QUERY_STRING", "<DNETARG>", &length, &value );
    if ( (status&1) == 0 ) return status;
    if ( length > 0 ) {
	length = length - 1;		/* remove leading '?' */
	for ( i = 0; i < length; i++ ) value[i] = value[i+1];
#ifdef DECODE_QUERY
        net_unescape_string ( value, &length );
#endif
	value[length] = '\0';
    }
    /*
     * Get header lines from HTTP server and save in environment array.
     */
    status = net_link_write ( "<DNETHDR>", 9 );
    if ( (status&1) == 0 ) return status;
    content_length = 0;
    do {
	status = net_link_read ( buffer, sizeof(buffer), &length );
	if ( (status &1) == 0 ) return status;
	/* Parse out header label */
	for ( colon = 0; colon < length; colon++ ) if ( buffer[colon]==':' ) {
	    value = malloc ( length+1 );
	    strncpy ( value, buffer, length );
	    value[length] = '\0';
	    /*
	     * Construct label.  Upcase characters and convert '-' to '_'.
	     */
	    label = malloc ( colon + 6 );
	    
	    strcpy ( label, "HTTP_" );
	    for ( i = 0; i < colon; i++ ) {
		label[i+5] = _toupper(buffer[i]);
		if ( label[i+5] == '-' ) label[i+5] = '_';
	    }
	    label[colon+5] = '\0';
	    /*
	     * Trim leading whitespace.
	     */
	    for ( colon++; isspace(value[colon]); colon++ );
	    /*
	     * Check for special header lines that go into pre-defined
	     * variables.
	     */
	    if ( 0 == strcmp(label,"HTTP_CONTENT_LENGTH") ) {
		ndx = env_index ( "CONTENT_LENGTH" );
		content_length = atoi ( &value[colon] );
		free ( label );
	    } else if ( 0 == strcmp ( label, "HTTP_CONTENT_TYPE") ) {
		ndx = env_index ( "CONTENT_TYPE" );
		free ( label );
	    } else if ( (ndx=env_index(label)) < ENV_LIMIT ) {
		/*
		 * Append string to existing.  Allocate extra.
		 */
		int old_length;
		char *cat_value;
		old_length = strlen ( env[ndx].value );
		cat_value = malloc ( old_length + length + 3 );
		strcpy ( cat_value, env[ndx].value );
		strcpy ( &cat_value[old_length], ", ");
		strcpy ( &cat_value[old_length+2], &value[colon] );

		free ( env[ndx].value );
		free ( value );
		value = cat_value;
		colon = 0;
	    } else {
		/* Make new */
		ndx = env_used;
		if ( ndx < ENV_LIMIT ) { 
		    env_used = ndx + 1; 
		    env[ndx].name = label;
		}
	    }
	    if ( ndx < ENV_LIMIT ) {
		char *in, *out;
		env[ndx].state = 1;		/* mark as valid */
		env[ndx].value = value;
		/* Trim label from value buffer. */
		if ( colon != 0) {
		    in = &value[colon]; 
		    for ( out = value; *in; *out++ = *in++ );
		    *out = '\0';
		}
	    }
	    break;
	}

	if ( (colon >= length) && (length > 0) ) {
	   /* Continuation header line, handle later */
	}
    } while ( length > 0 );	/* continue until null line read */
    /*
     * If request includes data, read it into temporary file.
     */
    if ( content_length > 0 ) {
	if ( contentf = fopen ( tmpnam(NULL), "w+", "fop=dlt", "mbc=64" ) ) {
	    int remaining;
	    /*
	     * Request the file a chunk at a time.
	     */
	    for ( remaining = content_length; remaining > 0;
		  remaining = remaining - length ) {
		/*
		 * Get server to read more data from client and sent it to us.
		 */
	        status = net_link_write ( "<DNETINPUT>", 11 );
		if ( (status&1) == 0 ) break;
		/*
		 * Read the sent data and output to contents file.
		 */
		status = net_link_read ( buffer, sizeof(buffer), &length );
		if ( (status&1) == 0 ) break;
		if ( length > 0 ) status = fwrite(buffer, length, 1, contentf);

		if ( status == 0 ) { status = vaxc$errno; break; }/* error */
	    }
	    if ( remaining > 0 ) {
		fprintf ( stderr,
			"Error getting request contents: %s\n(continuing)\n",
			strerror ( EVMSERR, status ) );
	    }
	    /*
	     * Reset for reads.
	     */
	    fseek ( contentf, 0, 0 );
	} else {
	    fprintf(stderr,"Error opening request contents temp file:\n%s\n",
			strerror(errno,vaxc$errno) );
	    content_length = 0;
	}
    }
    /*
     * Tell server to expect CGI response.
     */
    status = net_link_write ( "<DNETCGI>", 9 );
    net_link_set_rundown ( "</DNETCGI>" );
    return status;
}
/************************************************************************/
/* Return CGI variable.  In standard CGI this would just be a getenv call.
 * The lookup will ignore a "WWW_" prefix, if present, on the variable name
 * to ease porting of CERN VMS clients.
 */
char *cgi_info ( char *name ) 
{
    int ndx;

    if ( strncmp ( name, "WWW_", 4 ) == 0 )  name = &name[4];
    ndx = env_index ( name );
#ifdef DEBUG
printf("/cgilib/ info index for %s is %d\n", name, ndx );
#endif
    if ( ndx < env_used ) return env[ndx].value;

    return getenv ( name );
}
/*****************************************************************************/
/* Write data to CGI result file.  For CERN server this would go to
 * standard output.  Return value is number of characters written or -1;
 */
int cgi_write ( char *buffer, int bufsize )
{
    int status, remaining, length, segment;
    /*
     * Network link can't handle arbitrary write lengths, break it up.
     */
    for (remaining = bufsize; remaining > 0; remaining = remaining - length) {
	length = remaining > NET_IO_SIZE ? NET_IO_SIZE : remaining;
	status = net_link_write ( buffer, bufsize );
        if ( (status&1) == 0 ) break;    /* error */
	buffer += length;
    }
    return (bufsize - remaining);
}
/*****************************************************************************/
/* Read data from contents file.  In unix CGI this would read from standard
 * input.  Return value is number of character read or zero for eof.
 */
int cgi_read ( char *buffer, int bufsize )
{
    int status, length;
    if ( content_length <= 0 ) return 0;	/* no data in request */
    return fread ( buffer, 1, bufsize, contentf );
}
/************************************************************************/
/* Convert internal environment list to global DCL symbols with the
 * specified prefix.  Number of symbols created is returned.  The
 * rundown string in the exit handler is reset and a DCL symbol
 * WWWEXEC_RUNDOWN_STRING, is defined as well.  Any content file data
 * will be lost when program exits (not available from DCL).
 */
int cgi_set_dcl_env ( char *prefix )
{
    int i, hits, status, prefix_len, length, table, LIB$SET_SYMBOL();
    $DESCRIPTOR(symbol,"");
    $DESCRIPTOR(value,"");
    char symname[256];
    /*
     * Define global symbol used by WWWEXEC to terminate connection properly.
     */
    symbol.dsc$a_pointer = symname;
    strcpy ( symname, "WWWEXEC_RUNDOWN_STRING" );
    symbol.dsc$w_length = strlen(symbol.dsc$a_pointer);
    value.dsc$a_pointer = "</DNETCGI>";
    value.dsc$w_length = strlen(value.dsc$a_pointer);

    table = LIB$K_CLI_GLOBAL_SYM;
    status = LIB$SET_SYMBOL ( &symbol, &value, &table );
    if ( (status&1) == 1 ) net_link_set_rundown ( "" );	/* eliminate rundown output */
    else printf("Error in set_symbol: %d, len: %d\n", status,
	symbol.dsc$w_length );
    /*
     * Define local symbols to be used by calling script.
     */
    table = LIB$K_CLI_LOCAL_SYM;
    prefix_len = strlen ( prefix );
    if ( prefix_len > 255 ) prefix_len = 255;
    strncpy ( symname, prefix, prefix_len );

    for ( hits = i = 0; i < env_used; i++ ) if ( env[i].state ) {
	/*
	 * Construct symbol name.
	 */
	length = strlen ( env[i].name );
	if ( (prefix_len + length) > 255 ) length = 255 - prefix_len;
	symbol.dsc$w_length = prefix_len + length;
	strncpy ( &symname[prefix_len], env[i].name, length );
	/*
	 * Make descriptor for value.
	 */
	value.dsc$a_pointer = env[i].value;
	value.dsc$w_length = strlen ( env[i].value );
	if ( value.dsc$w_length > 255 ) value.dsc$w_length = 255;

	status = LIB$SET_SYMBOL ( &symbol, &value, &table );
    }
    return hits;
}
/*****************************************************************************/
/* Dump environement array to specified output file.
 */
int cgi_show_env ( int (*user_printf)() )
{
    int i, hits;
    for ( hits = i = 0; i < env_used; i++ ) {
	user_printf("%s %s%s\n", env[i].name, 
		env[i].state ? "= ": "*undefined* ", env[i].value);
	if ( env[i].state ) hits++;
    }
    return hits;
}
/**************************************************************************/
/* Provide formatted output to net_link, replacing printf calls to stdout.
 * Newline chars in format string are replaced with carriage-return/linefeed
 * pairs.
 */
int cgi_printf ( const char *fmt, ... )
{
    va_list alist;
    int state, out_len, status, j;
    char buffer[8192];
    /*
     * Setup pointer to variable argument list.
     */
    va_start ( alist, fmt );
    status = vsprintf ( buffer, fmt, alist );
    if ( status == EOF ) return status;
    va_end ( alist );

    status = net_link_printf ( "%s", buffer );
    return status;
}
