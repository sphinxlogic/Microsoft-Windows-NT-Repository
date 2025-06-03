/*
 * This program is a 'helper' program for the WWWEXEC DECnet object.
 * It opens a channel to netlink and saves the header and post data to a
 * file.
 *
 * usage:
 *    request_to_file dnet outfile [save_data]
 */
#include <stdlib.h>
#include <stdio.h>
#include <unixio.h>
#include <file.h>
#include <string.h>
#include <ctype.h>
int main ( int argc, char **argv )
{
    FILE *dnet, *reqf;
    int dnet_n, reqf_n;
    int length, content_length;
    char *inp, *colon, buffer[4096], content_type[4096];
    /*
     * Validate arguments and open files.  Note that server uses implicit
     * carriage return when responding to <DNETHDR> (allows command procs to
     * read header), so we open the connection in record acces ("ctx=rec") to
     * make read() function append line feed.
     */
    if ( argc < 3 ) {
	fprintf( stderr, "Missing arguments, usage: prog dnet_link outfile\n");
	exit ( 1 );
    }
    dnet_n = open ( argv[1], O_RDWR, 0, "ctx=rec" );
    if ( dnet_n < 0 ) { perror("can't open dnet_link"); exit ( 1 ); }

    reqf_n = open ( argv[2], O_CREAT | O_WRONLY, 0, "mbc=32" );  /* output file */
    if ( reqf_n < 0 ) { perror("Can't open request file"); exit(1); }
    /*
     * Send DNETHDR command over connection to HTTP server.
     */
    if ( write ( dnet_n, "<DNETHDR>", 9 ) != 9 ) {
	perror ( "dnet write failure" ); exit ( 1 );
    }
    /*
     * Read response back and add to file.  Last line in response will
     * be a zero length line (single line feed).
     */
    content_type[0] = '\0';
    content_length = -1;
    for( buffer[0] = 0; buffer[0] != '\n'; ) {
	/*
	 * Read line from server and echo to output file.
	 */
	length = read ( dnet_n, buffer, sizeof(buffer)-1 );
	if ( length <= 0 ) break;
	if ( length != write ( reqf_n, buffer, length ) ) {
	    perror ( "reqf write failure" ); break;
	}
	/*
	 * Find label delimiter and lowercase label for caseless compares.
	 */
	buffer[length] = '\0';
	colon = strchr ( buffer, ':' );
	if ( colon == NULL ) continue;
	for ( inp = buffer; inp < colon; inp++ ) *inp = _tolower(*inp);
	*colon++ = '\0';
	/*
	 * Save content-type and content-length header lines if found.
	 */
	if ( 0==strcmp ( buffer, "content-type") ) {
	   printf("found content-type: %s", colon );
	    strcpy ( content_type, colon );
	} else if ( 0==strcmp(buffer,"content-length") ) {
	    content_length = atoi ( colon ); 
	   printf("found content-length(%d): %s", content_length, colon );
	}
    }
    if ( length < 0 ) perror ( "dnet read failure" );
    else if ( content_length > 0 ) {
	int remaining;
	/*
	 * Document supplies a content-length, read that many char.
	 * Re-open network link with "rfm=var" to suppress LF insertion by RTL.
	 */
	close ( dnet_n );
	dnet_n = open ( argv[1], O_RDWR, 0, "ctx=rec", "rfm=var" );
	if ( !dnet_n ) length = 0;
	/*
	 * Prompt for data and copy to file.
	 */
	printf("Reading %d data bytes\n", content_length );
	for ( remaining = content_length; (remaining > 0) && (length > 0); 
		remaining = remaining - length ) {
	    if ( write ( dnet_n, "<DNETINPUT>", 11 ) != 11 ) {
	        perror ( "dnet write failure" ); exit ( 1 ); 
	    }
	    length =  read ( dnet_n, buffer, sizeof(buffer)-1 );
	    if ( length > 0 ) write ( reqf_n, buffer, 
		length > remaining ? remaining : length );
	}
    }
    close ( dnet_n );
    close ( reqf_n );
}
