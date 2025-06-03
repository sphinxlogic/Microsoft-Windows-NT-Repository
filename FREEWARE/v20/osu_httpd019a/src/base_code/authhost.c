/*
 * Define routine to lookup hostname given it's IP address.
 */
#include <stdio.h>
#include <stdlib.h>
#include <socket.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>

void lookup_hostname ( unsigned char *address, char *hostname )
{
    struct hostent *curhost;
    int i;

    curhost = gethostbyaddr ( (char *) address, 4, AF_INET );
    fprintf(stderr,"gethostaddr result: %d\n", curhost );
    if ( curhost ) if ( curhost->h_name ) {
	fprintf(stderr,"Offical name: '%s'\n", curhost->h_name );
	for ( i = 0; (i < 255) && (curhost->h_name[i]); i++ ) {
	    hostname[i] = _toupper ( curhost->h_name[i] );
	}
	hostname[i] = '\0';
	return;
    }

    strcpy ( hostname, "-1.-1" );
}
