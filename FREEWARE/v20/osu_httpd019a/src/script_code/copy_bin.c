/*
 * Copy file argument given by argv[1] to net_link via scriptlib.
 */
#include <stdio.h>
#include <stdlib.h>
#include "scriptlib.h"

int main ( int argc, char **argv )
{
    FILE *fp;
    int status, length;
    char buffer[4096];

    if ( argc < 2 ) { fprintf ( stderr,"Missing argument (filename)\n" );
	exit(20); }
    fp = fopen ( argv[1], "rb" );
    if ( !fp ) { perror ( "Open error on file" ); exit(1); }

    status = net_link_open();
    if ( (status&1) == 0 ) exit ( status );

    while ( (status&1) == 1 ) {
	length = fread ( buffer, sizeof(char), sizeof(buffer), fp );
	if ( length <= 0 ) break;
	status = net_link_write ( buffer, length );
    }
    net_link_close();
    exit(status);
}
