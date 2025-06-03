/*
 * Test program to exercise the message_service routines.
 */
#include "pthread_np.h"
#include "tutil.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef D_SIZE
#define D_SIZE 24
#endif

#include "message_service.h"

int howdy_world ( mst_link_t link, char *port, char *info, int ndx, int avail )
{
   int status, length, length2;
   char message[128], buffer[1000];

   status = mst_close (link );
   return status;
}
int hello_world ( mst_link_t link, char *port, char *info, int ndx, int avail )
{
   int wsize, status, length, length2;
   char message[128], buffer[1000];
   mst_link_t link2;
printf("/hello_world/ connection %s/%d avail: %d, info: '%s'\n", port,
 ndx, avail, info );

    switch ( *info ) {
      case 'a':
	status = mst_write ( link, "hello world", 12, &length );
   	mst_format_error ( status, message, sizeof(message) );
   	printf("/hello_world/ status of write: %d -> '%s' (%d)\n", status, 
		1700000000/(tu_strlen(info)-1), message, message );
	break;
      case 'b':
	status = mst_read ( link, buffer, sizeof(buffer), &length );
   	mst_format_error ( status, message, sizeof(message) );
   	printf("/hello_world/ status of read: %d -> '%s'\n", status, message );
	break;
      case 'c':
	status = mst_connect ( "hello", &info[1], &link2 );
	printf("/hello_world/ status of subconect: %d, (%d)\n", status,
		message );
	status = mst_read ( link2, buffer, sizeof(buffer), &length2 );
	printf("/hello_wrold/ staus of subread: %d %d\n", status, length2 );
	if ( status = MST_NORMAL ) {
		status = mst_write ( link, buffer, length2, &length );
	    printf("/hello_world/ status of subwrite: %d %d\n", status, length );
	}
	break;
	case 'd':
	    tu_strcpy ( buffer, "Did this sink in yet" );
	    wsize = tu_strlen ( buffer );
	    if ( info[1] ) wsize = atoi(&info[1]);
printf("wsize: %d info: '%s'\n", wsize, &info[1] );
	    while ( MST_NORMAL == mst_write ( link, buffer, wsize,
		&length ) );
	status = mst_close ( link );
	break;
	case 'e':
	status = mst_close (link );
	break;
    }
   return 1;
}

int main ( int argc, char **argv )
{
    mst_link_t link;
    int status, pool, pool2, length, repeat;
    char message[128], buffer[1000];

    printf("MSTVERIFY startup\n");
    status = mst_create_service_pool ( pthread_attr_default,
	5, 0, &pool );
    mst_format_error ( status, message, sizeof(message) );
    printf("Pool create status: %d -> '%s', id: %d\n", status, message, pool );

    status = mst_register_service ( "hello", hello_world, "main thread", pool);
    mst_format_error ( status, message, sizeof(message) );
    printf("register service status: %d -> '%s'\n", status, message );

    if ( argc > 2 ) repeat = atoi ( argv[2] ); else repeat = 0;
	printf("repeat count: %d\n", repeat );
    if ( repeat > 0 ) {
	int LIB$INIT_TIMER(), LIB$SHOW_TIMER();
        status = mst_connect ( "hello", argv[1][0] == 'd' ? argv[1] : "d", 
		&link );
        mst_format_error ( status, message, sizeof(message) );
        printf("connect status: %d -> '%s'\n", status, message );
	status = mst_read ( link, buffer, 1000, &length );
	LIB$INIT_TIMER();
	if ( status == MST_NORMAL ) for ( ; repeat > 0; --repeat ) {
	   status = mst_read ( link, buffer, 1000, &length );
	    if ( status != MST_NORMAL ) break;
	}
	LIB$SHOW_TIMER();
	printf("buffer: %s\n", buffer );
    } else if ( repeat < 0 ) {
	int LIB$INIT_TIMER(), LIB$SHOW_TIMER();
        status = mst_create_service_pool ( pthread_attr_default,
	    argc > 3 ? atoi(argv[3]) : 50, 1, &pool2 );
        mst_format_error ( status, message, sizeof(message) );
        printf("Pool create status: %d -> '%s', id: %d\n", status, message, 
		pool );
        status = mst_register_service ( "howdy", howdy_world, "main thread", 
		pool2 );
        mst_format_error ( status, message, sizeof(message) );
        printf("register service status: %d -> '%s'\n", status, message );
	LIB$INIT_TIMER();
	for ( repeat = (-repeat); repeat > 0; --repeat ) {
            status = mst_connect ( "howdy", "e", &link );
	    if ( status != MST_NORMAL ) break;
	    status = mst_close ( link );
	    if ( status != MST_NORMAL ) break;
	}
	LIB$SHOW_TIMER();
        mst_format_error ( status, message, sizeof(message) );
        printf("final status: %d -> '%s'\n", status, message );
    }

    status = mst_connect ( "hello", argc  > 1 ? argv[1] : "dummy", &link );
    mst_format_error ( status, message, sizeof(message) );
    printf("connect status: %d -> '%s'\n", status, message );

    status = mst_write ( link, buffer, sizeof(buffer), &length );
    mst_format_error ( status, message, sizeof(message) );
    printf("write status: %d -> '%s', length: %d\n", status, message, length );
    if ( status == MST_NORMAL ) printf ( "data read: '%s'\n", buffer );

    status = mst_close ( link );
    mst_format_error ( status, message, sizeof(message) );
    printf("close status: %d -> '%s'\n", status, message );

   
    status = mst_connect ( "hello", "dummy2", &link );
    mst_format_error ( status, message, sizeof(message) );
    printf("connect status: %d -> '%s'\n", status, message );

    status = mst_read ( link, buffer, sizeof(buffer), &length );
    mst_format_error ( status, message, sizeof(message) );
    printf("read status: %d -> '%s', length: %d\n", status, message, length );
    if ( status == MST_NORMAL ) printf ( "data read: '%s'\n", buffer );

    
    return 1;
}
