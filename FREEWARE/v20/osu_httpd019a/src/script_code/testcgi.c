/*
 * Verify operation of cgilib routines.
 */
#include <stdio.h>

#include "cgilib.h"
int cgi_show_env();

int main ( int argc, char **argv )
{
    int status;

    status = cgi_init ( argc, argv );
    cgi_printf("content-type: text/plain\n\n");	/* CGI header */
    cgi_printf("status of cgi_init: %d\n", status );

    status = cgi_show_env ( cgi_printf );
    cgi_printf("final status of show_env: %d\n", status );

    return 1;
}
