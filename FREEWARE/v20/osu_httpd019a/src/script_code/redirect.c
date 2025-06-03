/*
 * Presentation script for doing automatic re-directs using the contents
 * of the specified file as the URL.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scriptlib.h"
static int error_abort ( char *status_line, char *message );

int main ( int argc, char **argv )
{
    int status, length;
    FILE *ufile;
    char *cp, line[1024];
    /* Connect to net_link */
    if ( argc < 3 ) {
	fprintf( stderr, "Missing arguments, usage: prog method url\n");
	exit ( 1 );
    }
    status = net_link_open();
    if ( (status&1) == 0 ) { printf("Net_link_open error\n"); exit(status); }
    /*
     * Open URL file and read first line.
     */
    ufile = fopen ( argv[2], "r" );
    if ( !ufile ) error_abort ( "404 open failure", argv[2] );
    cp = fgets ( line, sizeof(line)-1, ufile );
    fclose ( ufile );
    if ( !cp ) error_abort ( "500 file read error", "Error reading URL file" );
    /*
     * Send redirect.
     */
    status = net_link_write ( "<DNETCGI>", 9 );
    status = net_link_set_rundown ( "</DNETCGI>" );
    status = net_link_printf ( "status: 302 Redirect\nlocation: %s\n\n", line );
    status = net_link_close();
}
/**************************************************************************/
static int error_abort ( char *status_line, char *message )
{
    int written, length, status;
    status = net_link_write ( "<DNETTEXT>", 10 );
    status = net_link_write ( status_line, strlen(status_line) );
    status = net_link_write ( message, strlen(message) );
    status = net_link_write ( "</DNETTEXT>", 11 );
    status = net_link_close();
    exit ( 1 );
    return 1;
}
