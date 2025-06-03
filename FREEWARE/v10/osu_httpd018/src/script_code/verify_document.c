/*
 * This routine connects to an HTTP server and does a HEAD request to
 * determine is a particular file exists.
 *
 * Arguments:
 *	host/port.
 *
 * Author:	David Jones
 * Date:	10-DEC-1994
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unixio.h>
#include <errno.h>
#include <types.h>
#include <socket.h>
#include <in.h>
#include <netdb.h>
int verify_document ( char *host, char *ident )
{
    int mgr_port, status, sd, i, j, rem_port, length;
    char *target;
    struct sockaddr local, remote;
    struct hostent *hostinfo;
    char remote_host[256], response[1024];
    /*
     * Parse host string for port.
     */
    rem_port = 80;
    for ( i = 0; (i < 255) && host[i]; i++ ) {
	if ( host[i] == ':' ) {
	    rem_port = atoi ( &host[i+1] );
	    break;
	}
	remote_host[i] = host[i];
    }
    remote_host[i] = '\0';
    if ( i == 0 ) strcpy ( remote_host, "localhost" );
    /*
     * Send command to all targets, default if none specified.
     */
    /*
     * Create socket on randome port
     */
    sd = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( sd < 0 ) {
        fprintf(stderr,"Error creating socket: %s\n", strerror(errno) );
	return 0;
    }
    local.sa_family = AF_INET;
    for ( j = 0; j < sizeof(local.sa_data); j++ ) local.sa_data[j] = '\0';
    local.sa_data[0] = local.sa_data[1] = 0;
    status = bind ( sd, &local, sizeof ( local ) );
    if ( status < 0 ) {
	    fprintf(stderr,"Error binding socket: %s\n", strerror(errno) );
	return 0;
    }
    /*
     * Lookup host.
     */
    hostinfo = gethostbyname ( remote_host );
    if ( !hostinfo ) {
	fprintf(stderr, "Could not find host '%s'\n", remote_host );
	return 0;
    }
    remote.sa_family = hostinfo->h_addrtype;
    for ( j = 0; j < hostinfo->h_length; j++ ) {
	    remote.sa_data[j+2] = hostinfo->h_addr_list[0][j];
    }
    remote.sa_data[0] = rem_port >> 8;
    remote.sa_data[1] = (rem_port&255);
    /*
     * Connect to remote host.
     */
    status = connect ( sd, &remote, sizeof(remote) );
    if ( status == 0 ) {
	int i, j, cr;
	char ch;
	/*
	 * Send request followed by newline.
	 */
	sprintf ( response, "HEAD %s HTTP/1.0\r\n\r\n", ident );
	status = send ( sd, response, strlen(response), 0 );
	if ( status < 0 ) {
		fprintf(stderr,"status of write: %d %d\n", status, errno );
	    return 0;
	}
	/*
	 * Read response and save first line.
	 */
	cr = 0;
	response[1023] = '\0';
	for ( i = 0; !cr && (i < 1023); i+= length ) {
	    length = recv ( sd, &response[i], 1023-i, 0 );
	    if ( length > 0 ) {
		for ( j = i; j < i+length; j++ ) {
		    if ( response[j] == '\r' || response[j] == '\n' ) {
			response[j] = '\0';
			cr = 1;
			break;
		    }
		}
	    } else {
		response[i] = '\0';
		length = 0;
		cr = 1;
		break;
	    }
	}
	length = i;
	/*
	 * Read and discard rest of response.
	 */
	while (0 < recv(sd, &response[length+1], 1023-length, 0 ) ) {
	}
    } else {
        fprintf(stderr, "error connecting to '%s': %d/%d\n", remote_host,
		status, errno );
	response[0] = '\0';
	length = 0;
    }
    close ( sd );
#ifdef TESTIT
    printf("stsline(%d): %s\n", length, response );
#endif
    /*
     * Check status on HTTP response line. "HTTP/1.0 200"
     */
    if ( length >= 12 ) if ( 0 == strncmp ( response,"HTTP/1.0 ",9) ) {
	for ( i = 9; isspace(response[i]); i++ );  /* skip whitespace */
	if ( response[i] == '2' ) return 1;
    }
    return 0;
}
#ifdef TESTIT
int main ( int argc, char **argv )
{
    int status;
    status = verify_document ( argv[1], argv[2] );
   printf("verify_document status: %d\n", status );
}
#endif
