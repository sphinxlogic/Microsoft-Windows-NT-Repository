/*
 * Verify operation of cgilib routines.
 */
#include <stdio.h>

#include "cgilib.h"

int main ( int argc, char **argv )
{
    int status;

    status = cgi_init ( argc, argv );
    cgi_printf("content-type: text/plain\r\n");	/* CGI header */
    cgi_printf("status: 401 demo for Jason\r\n");
    cgi_printf("WWW-authenticate: basic realm=\"PH\"\r\n\r\n");

    return 1;
}
