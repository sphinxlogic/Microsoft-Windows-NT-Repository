/*
 * Verify operation of cgilib routines.
 */
#include <stdio.h>
#include <stdlib.h>

#include "cgilib.h"
int cgi_show_env();

int main ( int argc, char **argv )
{
    int status;
    int content_length;
    char *content_data;

    status = cgi_init ( argc, argv );
    cgi_printf("content-type: text/plain\n\n");	/* CGI header */
    cgi_printf("status of cgi_init: %d\n", status );

    status = cgi_show_env ( cgi_printf );
    cgi_printf("final status of show_env: %d\n", status );

    cgi_printf(
	"cgi_info content_legth addr: %d\n", cgi_info ( "CONTENT_LENGTH" ));

    content_length = atoi(cgi_info ("CONTENT_LENGTH"));
    fprintf(stderr,"Content length: %d\n", content_length );
    if (content_length == 0)
    {
	return 1;
    }

    content_data = (char *) malloc(sizeof(char)*(content_length+1));
    status = cgi_read(content_data, content_length);

    cgi_printf("read status   : %d\n", status);

    cgi_printf("content length: %d\n", content_length);
    cgi_printf("content text  : %s\n", content_data);

    return 1;
}
