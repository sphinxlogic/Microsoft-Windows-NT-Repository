/*
//  The DECwindows IO$M_WS_DISPLAY I/O codes are neither documented
//  nor supported.
//
//  $ CC/DECC DECW$SHOW_DISPLAY
//  $ LINK DECW$SHOW_DISPLAY
//  $ RUN DECW$SHOW_DISPLAY
*/
#include <descrip.h>
#include <iodef.h>
#include <lib$routines.h>
#include <ssdef.h>
#include <starlet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stsdef.h>

#define IO$M_WS_DISPLAY         64
#define DECW$C_WS_DSP_NODE       1
#define DECW$C_WS_DSP_TRANSPORT  2
#define DECW$C_WS_DSP_SERVER     3
#define DECW$C_WS_DSP_SCREEN     4

#define RETBUFSIZ 80

main()
    {
    int RetStat;
    short IOChan;
    unsigned short int IOSB[2];
    short int RetLen;
    char RetBuf[RETBUFSIZ];
    $DESCRIPTOR( DECwDevice, "DECW$DISPLAY" );

    RetStat = sys$assign( &DECwDevice, &IOChan, 0, 0 );
    if ( !$VMS_STATUS_SUCCESS( RetStat ))
	lib$signal( RetStat );

    RetStat = sys$qiow( 0, IOChan,
	IO$_SENSEMODE | IO$M_WS_DISPLAY,
        IOSB, 0, 0, RetBuf, RETBUFSIZ - 1,
	DECW$C_WS_DSP_TRANSPORT, 0, 0, 0 );
    if ( !$VMS_STATUS_SUCCESS( RetStat ))
	lib$signal( RetStat );
    if ( !$VMS_STATUS_SUCCESS( IOSB[0] ))
	lib$signal( IOSB[0] );

    RetLen = IOSB[2];
    printf( "Transport:  %*.*s\n", RetLen, RetLen, RetBuf );

    return SS$_NORMAL;
    }
