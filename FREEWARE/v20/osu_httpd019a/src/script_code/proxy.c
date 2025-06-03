/*
 * This program is a CGI script that proxies HTTP requests to a 
 * remote host.  The path portion of the requested URL is parsed into
 * a host and URL.
 *
 * Author:	David Jones
 * Date:	2-NOV-1994
 */
#include <stdlib.h>
#include <unixio.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <errno.h>
#include <ctype.h>
#include <socket.h>
#include <in.h>
#include <netdb.h>
#include "scriptlib.h"		/* net_link_... routines */

static int open_remote(), send_query(), read_response();
/********************************************************************/
/* Main routine.
 */
int main ( int argc, char **argv )
{
    int sd, tcnt, length, i, status;
    char *inp;
    char bin_path[256], remhost[256];
    /*
     * Validate command line arguments.  The target machine
     * is appended to the script name that invokes this program.
     */
    if ( argc < 3 ) {
	fprintf( stderr, "Missing arguments, usage: prog method url\n");
	exit ( 1 );
    }
    /*
     * Open network link and get search/point argument.  Note that network
     * connection is record oriented, a zero length record is allowed.
     */
    status = net_link_open();
    if ( (status&1) == 0 ) { perror("Network connect fail"); exit (status); }
    /*
     * Verify that URL starts with the appropriate path, then trim
     * off path and script name from the URL.
     */
    status = net_link_query 
		( "<DNETPATH>", bin_path, sizeof(bin_path)-1, &length );
    if ( (status&1) == 0 ) { perror("Network I/O failure"); exit (status); }
    if ( length < 0 ) exit ( 1 );

    bin_path[length] = '\0';		/* terminate string */

    if ( 0 != strncmp(argv[2], bin_path, length) ) {
	fprintf( stderr, 
	   "Invalid URL (%s), must start with %s\n",argv[2], bin_path);
	exit ( 1 );
    }
    for ( inp = &argv[2][length]; *inp && (*inp != '/'); inp++ );
    /*
     * Split off remote hostname.
     */
    if ( *inp ) {
	inp++;
	for ( i = 0; i < sizeof(remhost)-1; i++ ) {
	   if ( *inp && (*inp != '/') ) remhost[i] = *inp++;
	   else break;
	}
	remhost[i] = '\0';
    }
    else remhost[0] = '\0';
    /*
     * Connect to remote host.
     */
    sd = open_remote ( remhost );
    if ( sd < 0 ) {
    }
    /*
     * Construct query and send to remote.
     */
    status = send_query ( sd, argv[1], inp, argv[3] );
    if ( (status&1) == 1 ) status = read_response ( sd );
    return status;
}
/*****************************************************************************/
/* Create socket and connect to remote host.
 */
static int open_remote ( char *remhost )
{
    struct sockaddr local, remote;
    struct hostent *hostinfo;
    int i, j, rem_port, sd, status;
    char *alt_port;
    /* */
    rem_port = 80;

    alt_port = strchr ( remhost, ':' );
    if ( alt_port ) {
	*alt_port++ = '\0';
	rem_port = atoi ( alt_port );
    }
    hostinfo = gethostbyname ( remhost );
    if ( !hostinfo ) {
	    fprintf(stderr, "Could not find host '%s'\n", remhost );
	return -1;
    }
    /*
     * Attempt connect to remote host.
     */
    sd = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( sd < 0 ) {
	    fprintf(stderr,"Error creating socket: %s\n", strerror(errno) );
	return sd;
    }
    local.sa_family = AF_INET;
    for ( j = 0; j < sizeof(local.sa_data); j++ ) local.sa_data[j] = '\0';
    local.sa_data[0] = local.sa_data[1] = 0;
    status = bind ( sd, &local, sizeof ( local ) );
    if ( status < 0 ) {
	    fprintf(stderr,"Error binding socket: %s\n", strerror(errno) );
	return -1;
    }

    remote.sa_family = hostinfo->h_addrtype;
    for ( j = 0; j < hostinfo->h_length; j++ ) {
	    remote.sa_data[j+2] = hostinfo->h_addr_list[0][j];
    }
    remote.sa_data[0] = rem_port >> 8;
    remote.sa_data[1] = (rem_port&255);
    status = connect ( sd, &remote, sizeof(remote) );
    if ( status != 0 ) {
	return -1;
    }
    return sd;
}
/*****************************************************************************/
/* Get request information and send query.
 */
static int send_query ( int sd, char *method, char *path, char *version )
{
    int content_length, i, status, length, written;
    char *cmp, buffer[4096];
    /*
     *  Send initial part of request.
     */
    strcpy ( buffer, method );
    strcat ( buffer, " " ); strcat ( buffer, path );
    strcat ( buffer, " " ); strcat ( buffer, version );
    strcat ( buffer, "\r\n" );
    length = strlen ( buffer );
    written = write ( sd, buffer, length );
    if ( written != length ) return 0;
    /*
     * Fetch header lines.
     */
    cmp = "CONTENT-LENGTH:";
    content_length = 0;
    for ( status = net_link_query ( "<DNETHDR>", buffer, sizeof(buffer)-2,
	   &length ); ((status&1) == 1) && (length > 0);
           status = net_link_read ( buffer, sizeof(buffer)-2, &length ) ) {
	/*
	 * Relay to remote.
	 */
	buffer[length++] = '\r';
	buffer[length++] = '\n';
	written = write ( sd, buffer, length );
        if ( written != length ) return 0;
 	/*
	 * Check if line is 'content-length:' and save.
	 */
	buffer[length-2] = '\0';
	for ( i = 0; i < length; i++ ) {
	    if ( cmp[i] != _toupper(buffer[i]) ) break;
	    if ( cmp[i] == ':' ) {
fprintf(stderr,"Found content_length: %s\n", buffer);
		content_length = atoi ( &buffer[i+1] );
		break;
	    }
	}
    }
    written = write ( sd, "\r\n", 2 );
    /*
     * Send additional.
     */
    while ( content_length > 0 ) {
	status = net_link_query ( "<DNETINPUT>", buffer, 
		sizeof(buffer), &length );
	if ( (status&1) == 0 ) break;
	if ( length > 0 ) {
	    written = write ( sd, buffer, length );
	    if ( written != length ) return 0;
	    content_length = content_length - length;
	}
    }
    return 1;
}
/*****************************************************************************/
static int read_response ( int sd )
{
    int status, length;
    char buffer[4096];

    status = net_link_write ( "<DNETRAW>", 9 );
    if ( (status&1) == 1 ) status = net_link_set_rundown ( "</DNETRAW>" );

    while ( (length = read ( sd, buffer, sizeof(buffer))) > 0 ) {
	status = net_link_write ( buffer, length );
	if ( (status&1) == 0 ) break;
    }
    return status;
}
