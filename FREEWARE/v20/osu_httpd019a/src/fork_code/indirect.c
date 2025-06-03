/*
 * Presentation script for doing automatic re-directs using the contents
 * of the specified file as the URL.  Assumed to be run as a CGI script.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int error_abort ( char *status_line, char *message );

int main ( int argc, char **argv )
{
    int status, length;
    FILE *ufile;
    char *path, *cp, line[1024];
    /*
     * Open URL file and read first line.
     */
    path = getenv ( "PATH_TRANSLATED" );
    if ( !path ) error_abort ( "500 internal error",
		"Internal error, missing environment variable" );
    ufile = fopen ( path, "r" );
    if ( !ufile ) error_abort ( "404 open failure", path );
    cp = fgets ( line, sizeof(line)-1, ufile );
    fclose ( ufile );
    if ( !cp ) error_abort ( "500 file read error", "Error reading URL file" );
    /*
     * Send redirect.
     */
    printf ( "status: 302 Redirect\nlocation: %s\n\n", line );
    return 0;
}
/**************************************************************************/
static int error_abort ( char *status_line, char *message )
{
    int written, length, status;
    printf("content-type: text/plain\nstatus: %s\n\n", status_line );
    printf("%s\n", message );
    exit ( 1 );
    return 1;
}
