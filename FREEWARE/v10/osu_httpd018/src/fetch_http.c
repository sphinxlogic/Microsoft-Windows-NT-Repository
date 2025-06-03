/*
 * This program connects to the http server and perform a fetch
 * of the URL.
 *
 * Usage:
 *      $ fetch_http URL [outputfile] [-b][-s]
 *   
 *
 * Arguments:
 *	URL		URL to fetch.
 *      outputfile      File to store result in.
 *
 * Author:	David Jones
 * Revised:     13-JAN-1995 GJC@VILLAGE.COM. Add file output argument.
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
    int status, sd, i, j, timeout, in_hdr, rem_port, http_parse_url();
    int binary_mode, simple_request;
    struct sockaddr local, remote;
    struct hostent *hostinfo;
    char *arg_url, *arg_ofile, *service, *node, *ident, *arg, *remote_host;
    char url_buf[1024];
    /*
     * Extract arguments from command line.
     */
    arg_url = arg_ofile = NULL;
    binary_mode = simple_request = 0;
    for ( i = 1; i < argc; i++ ) {
	if ( argv[i][0] == '-' ) {
	   for ( j = 1; argv[i][j]; j++ ) {
		if ( argv[i][j] == 'b' ) binary_mode = 1;
		else if ( argv[i][j] == 's' ) simple_request = 1;
		else fprintf(stderr,"Unknown option ignored\n");
	   }
	} else if ( arg_url == NULL ) arg_url = argv[i];
	else if ( arg_ofile == NULL ) arg_ofile = argv[i];
	else {
	    fprintf(stderr,"extra arguments ignored\n");
	}
    }
    if ( !arg_url ) {
	fprintf(stderr,"Missing argument, usage:\n");
	fprintf(stderr,"   $ fetch_http URL [output-filename] [-b] [-s]\n");
	exit ( EXIT_FAILURE );
    }
    /*
     * Parse target.
     */
    status = http_parse_url ( arg_url, url_buf, &service, &remote_host,
		&ident, &arg );
    rem_port = 80;
    for ( j = 0; remote_host[j]; j++ ) if ( remote_host[j] == ':' ) {
	rem_port = atoi ( &remote_host[j+1] );
	remote_host[j] = '\0';
	break;
    }
    if ( !*remote_host ) remote_host = "/localhost";
    /*
     * Create socket on first available port.
     */
    sd = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( sd < 0 ) {
	    fprintf(stderr,"Error creating socket: %s\n", strerror(errno) );
    }
    local.sa_family = AF_INET;
    for ( j = 0; j < sizeof(local.sa_data); j++ ) local.sa_data[j] = '\0';
    status = bind ( sd, &local, sizeof ( local ) );
    if ( status < 0 ) {
	    fprintf(stderr,"Error binding socket: %s\n", strerror(errno) );
    }
    /*
     * Lookup host.
     */
    hostinfo = gethostbyname ( &remote_host[1] );
    if ( !hostinfo ) {
	fprintf(stderr, "Could not find host '%s'\n", remote_host );
	return EXIT_FAILURE;
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
	in_hdr = !simple_request;
	if ( in_hdr )
	    sprintf(response, "GET %s%s%s HTTP/1.0\r\n\r\n", ident, 
		*arg ? "?" : "", arg );
	else
	    sprintf(response, "GET %s%s%s\r\n", ident, 
		*arg ? "?" : "", arg );
	status = send ( sd, response, strlen(response), 0 );
	if ( status < 0 )
		fprintf(stderr,"status of write: %d %d\n", status, errno );
	/*
	 * Read and echo response.
	 */
	if ( status > 0 ) {
	    int i, j, cr, hdr_state;
	    char ch;
	    FILE *outf, *cur_outf;
	    cr = 0;
	    if (arg_ofile) {
		if (!(outf = fopen(arg_ofile,binary_mode?"wb":"w")))
		 { perror(arg_ofile);  exit(EXIT_FAILURE);}
	    }
	    else
	      outf = stdout;
	    cur_outf = (in_hdr) ? stdout : outf;
	    hdr_state = 0;
	    while (0 < (length=recv(sd, response, sizeof(response)-1,0))) {
		if ( binary_mode && !in_hdr ) {
		    fwrite ( response, 1, length, cur_outf );
		} else {
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
			if ( arg_ofile && (ch == '\n') && !cr ) {
			    if ( hdr_state ) { 
				in_hdr = 0; 
			        response[j] = '\0';
				if ( j > 0 ) fprintf (cur_outf,"%s",response);
				cur_outf = outf;
				if ( binary_mode ) { 
				    i++;
				    if ( i < length ) 
					fwrite (&response[i], 1, length-i,outf);
				    break;
				}
				j = 0;
			    }
			    hdr_state = 1;
			} else if ( !cr ) hdr_state = 0;
		    }
		    response[j] = '\0';
		    if ( !binary_mode ) fprintf(cur_outf,"%s", response );
		}
	    }
	    if ( outf != stdout ) fprintf(outf,"\n");
	    if (outf != stdout) fclose(outf);
	}
    } else {
	fprintf(stderr, "error connecting to '%s': %d/%d\n", remote_host,
		status, errno );
    }
    close ( sd );
    return EXIT_SUCCESS;
}
/***************************************************************************/
int http_parse_url 
	( char *url, 			/* locator to parse */
	char *info,			/* Scratch area for result pts*/
	char **service,			/* Protocol (e.g. http) indicator */
	char **node,			/* Node name. */
	char **ident,			/* File specification. */
	char **arg )			/* Search argument */
	
{
    int i, state;
    char *last_slash, *p, c, arg_c;
    /*
     * Copy contents of url into info area.
     */
    *service = *node = *ident = *arg = last_slash = "";

    for ( state = i = 0; (info[i] = url[i]) != 0; ) {
	c = info[i];
	switch ( state ) {
	    case 0:
		if ( c == ':' ) {
		    info[i] = '\0';	/* terminate string */
		    *service = info;
		    state = 1;
		}
	    case 1:
		if ( c == '/' ) {
		    *ident = last_slash = &info[i];
		    state = 2;
		}
		break;
	    case 2:
		state = 4;
		if ( c == '/' ) {	/* 2 slashes in a row */
		    *node = *ident;
		    state = 3;
		}
		else if ( (c == '#') || (c == '?') ) {
		    arg_c = c;
		    info[i] = '\0';
		    *arg = &info[i+1];
		    state = 5;
		}
		break;
	    case 3:			/* find end of host spec */
		if ( c == '/' ) {
		    state = 4;
		    *ident = last_slash = &info[i];
		    for ( p = *node; p < *ident; p++ ) p[0] = p[1];
		    info[i-1] = '\0';	/* Terminate host string */
		}
		break;
	    case 4:			/* Find end of filename */
		if ( c == '/' ) last_slash = &info[i];
		else if ( (c == '#') || (c == '?') ) {
		    arg_c = c;
		    info[i] = '\0';
		    *arg = &info[i+1];
		    state = 5;
		}
	    case 5:
		break;
        }
	i++;
    }
    /*
     * Insert arg delimiter back into string.
     */
    if ( **arg ) {
	char tmp;
	for ( p = *arg; arg_c; p++ ) { tmp = *p; *p = arg_c; arg_c = tmp; }
	*p = '\0';
    }
    return 1;
}
