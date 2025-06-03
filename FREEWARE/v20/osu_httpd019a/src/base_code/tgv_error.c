/*
 * Decode error status returned by Multinet device driver.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main ( int argc, char **argv ) {
    int code;
    if ( argc < 2 ) {
	fprintf(stderr,"Must supply status code\n");
    } else {
	code = atoi ( argv[1] );
	if ( 0 == (code & 0x08000) ) {
	    printf("Not valid return code, %d\n", code);
	} else {
	    code = (code&0x07FF8) / 8;
	    printf("Errno: %d, '%s'\n", code, strerror ( code ) );
	}
    }
    return 1;
}
