/*
 * This program connects to the http server from a privileged port and
 * writes a management request.  The port used must be coordinated with
 * the server startup.
 *
 * Usage:
 *      $ privrequest priv-port "request" [[host:]port ...]
 *
 * Arguments:
 *	priv-port	Local port number from which to make the request
 *	request		Commmand to send to server.  If command contains
 *			spaces, you must enclose it in quotes.
 *	host		Host name on which server is running, default: localhost.
 *	port		Port number to connect to, default: 80.
 *
 * If more that one host/port is specified, the same command is sent to
 * each in the order that they appear.
 *
 * Author:	David Jones
 * Date:	20-AUG-1994
 * Revised:	3-SEP-1994	fixed bugs with bind() call.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef VMS
#include <unixio.h>
#include <types.h>
#include <socket.h>
#include <in.h>
#else
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <errno.h>
#include <netdb.h>
int main ( int argc, char **argv )
{
    int mgr_port, status, sd, i, j, timeout, rem_port, CLI$GET_VALUE();
    char *target;
    struct sockaddr local, remote;
    struct hostent *hostinfo;
    char remote_host[256];
    /*
     * Extract arguments from command line.
     */
    if ( argc < 3 ) {
	fprintf(stderr,"Missing arguments, usage:\n");
	fprintf(stderr,"   $ privrequest local-port request [[host:]port]\n");
	exit ( 1 );
    }
    mgr_port = atoi ( argv[1] );
    if ( mgr_port < 0 ) {
	fprintf(stderr,"Invalid port number\n" );
	exit ( 1 );
    } else if (mgr_port > 1023) 
	fprintf(stderr,"local port is not privileged\n");

    timeout = atoi ( argv[2] );
    /*
     * Send command to all targets, default if none specified.
     */
    if ( argc == 3 ) target = "localhost:80";
    else target = argv[3];

    for ( i = 3; target; ) {
	/*
	 * Parse target.
	 */
	remote_host[255] = '\0';
	for ( j = 0; j < sizeof(remote_host)-1; j++ ) {
	    remote_host[j] = *target++;
	    if ( *target == ':' ) {
		rem_port = atoi ( target+1 );
		remote_host[j+1] = '\0';
		break;
	    } else if ( *target == '\0' ) {
		rem_port = atoi ( remote_host );
		strcpy ( remote_host, "localhost" );
		break;
	    }
	}
	/*
	 * Create socket on privileged port.
	 */
	sd = socket ( AF_INET, SOCK_STREAM, 0 );
	if ( sd < 0 ) {
	    fprintf(stderr,"Error creating socket: %s\n", strerror(errno) );
	}
	local.sa_family = AF_INET;
	for ( j = 0; j < sizeof(local.sa_data); j++ ) local.sa_data[j] = '\0';
	local.sa_data[0] = mgr_port >> 8;
	local.sa_data[1] = (mgr_port&255);
	status = bind ( sd, &local, sizeof ( local ) );
	if ( status < 0 ) {
	    fprintf(stderr,"Error binding socket: %s\n", strerror(errno) );
	}
	/*
	 * Lookup host.
	 */
	hostinfo = gethostbyname ( remote_host );
	if ( !hostinfo ) {
	    fprintf(stderr, "Could not find host '%s'\n", remote_host );
	    i++;
	    if ( i < argc ) target = argv[i]; else target = (char *) 0;
	    continue;
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
	    int length;
	    char response[1024];
	    /*
	     * Send request followed by newline.
	     */
	    status = send ( sd, argv[2], strlen(argv[2]), 0 );
	    if ( status < 0 )
		fprintf(stderr,"status of write: %d %d\n", status, errno );
	    status = send ( sd, "\r\n\r\n", 4, 0 );
	    /*
	     * Read and echo response.
	     */
	    if ( status > 0 ) {
		int i, j, cr;
		char ch;
		cr = 0;
		printf("Port %d response:\n", rem_port );
		while (0 < (length=recv(sd, response, sizeof(response)-1,0))) {
		    for ( i = j = 0; i < length; i++ ) {
			ch = response[i];
			if ( response[i] == '\r' ) {
			    if ( cr == 1 ) response[j++] = '\r';
			    cr = 1;
			} else if ( cr == 1 ) {
			    if ( ch != '\n' ) response[j++] = '\r';
			    response[j++] = ch;
			    cr = 0;
			} else {
			    response[j++] = ch;
			    cr = 0;
			}
		    }
		    response[j] = '\0';
		    printf("%s", response );
		}
		printf("\n");
	    }
	} else {
	    fprintf(stderr, "error connecting to '%s': %d(%s)\n", remote_host,
		status, strerror(errno) );
	}
	close ( sd );
	/*
	 * Update target string for next pass through loop.
	 */
	i++;
	if ( i < argc ) target = argv[i]; else target = (char *) 0;
    }
    return 1;
}
